//include BaseUtil.h instead of including directly.


//Property:
//	els: pointer to the buffer
//	buf: default buffer. If the needed size is larger, els points to the new buffer.
//	cap: capacity
#include <stdio.h>
template <class T>
class Vec {
public:
	explicit Vec(Allocator* _allocator = NULL) :allocator(_allocator) {
		els = buf;
		Reset();
	}
	//Specified by the caller, the caller needs to ensure that _len and _cap is reasonable
	Vec(T* _els, size_t _len, size_t _cap, Allocator* _allocator = NULL) {
		els = _els;
		len = _len;
		cap = _cap;
		allocator = _allocator;
	}
	//T must be POD type.
	Vec(const Vec& vec):Vec(){

		EnsureCapCrash(vec.cap);
		memcpy(els, vec.els, sizeof(T) * vec.len);
		len = vec.len;
	}
	~Vec(){
		FreeEls();
	}
protected:
	static const int PADDING = 1;
	size_t len;
	size_t cap;
	T* els;
	T buf[16];
	Allocator* allocator;

	void Reset() {
		len = 0;
		cap = dimof2(buf) - PADDING;
		els = buf;
		memset(buf, 0, sizeof(T) * cap);
	}
	void EnsureCapCrash(size_t needed) {
		bool ok = EnsureCapTry(needed);
		if (!ok)
			CrashMe();
	}
	bool EnsureCapTry(size_t needed) {
		if (needed < cap)
			return true;

		size_t newCap = cap * 2;
		if (needed > newCap)
			newCap = needed;

		size_t newElsNum = newCap + PADDING;
		if (newElsNum > UINT_MAX / sizeof(T))
			return false;

		size_t allocSize = newElsNum * sizeof(T);
		size_t paddingSize = allocSize - len * sizeof(T);
		T* newBuf;
		if (buf == els)
			newBuf = (T*)Allocator::MemDup(allocator, buf, len * sizeof(T), paddingSize);
		else
			newBuf = (T*)Allocator::ReAlloc(allocator, els, allocSize);

		cap = newCap;
		els = newBuf;
		memset(els + len, 0, paddingSize);
		return TRUE;

	}
	T* MakeSpaceAt(size_t idx, size_t count){
		size_t newLen = max(idx, len) + count;

		EnsureCapCrash(newLen);

		T* res = &els[idx];
		if (len > idx) {
			T* src = &els[idx];
			T* dst = &els[idx + count];
			memmove(dst, src, len - idx);
		}
		len = len + count;
		return res;
	}
	void FreeEls() {
		if (els != buf)
			Allocator::Free(allocator,els);
	}
	void FreeMembers() {
		for (int i = 0; i < len; i++) {
			auto tmp = els[i];
			free(tmp);
		}
		Reset();
	}
	//Only support POD types.
public:
	Vec& operator=(const Vec& that) {
		if (&that == this)
			return *this;

		FreeEls();
		EnsureCapCrash(that.cap);
		memcpy(els, that.els, sizeof(T) * that.len);
		len = that.len;
		memset(els + len, 0, sizeof(T) * (cap - len));
		return *this;
	}
	T& operator[](size_t idx) {
		if (idx >= len)
			CrashMe();
		return els[idx];
	}
	T& at(size_t idx) const {
		CrashIf(idx >= len);
		return els[idx];
	}
	size_t Size() {
		return len;
	}
	void InsertAt(size_t idx, const T& el) {
		MakeSpaceAt(idx, 1)[0] = el;
	}
	void Append(const T& el) {
		MakeSpaceAt(len, 1)[0] = el;
	}
	void Append(const T* src, size_t count) {
		if (count == 0)
			return;
		if (src == nullptr)
			CrashMe();

		T* dst=MakeSpaceAt(len, count);
		memcpy(dst, src, sizeof(T) * count);
	}
	//T is char* or wchar_t*
	void AppendStrPtr(const T src) {
		CrashIf(src == nullptr);
		T* dst=MakeSpaceAt(len, 1);
		*dst = (wchar_t*)Allocator::MemDup(allocator, src, str::Len(src)*sizeof(wchar_t),2);
	}
	T* AppendBlanks(size_t count) {
		return MakeSpaceAt(len, count);
	}
	void RemoveAt(size_t idx, size_t count = 1) {
		if (idx+count >= len)
			CrashMe();
		if (count == 0)
			return;

		T* src = els + idx + count;
		T* dst = els + idx;
		memmove(dst, src, count * sizeof(T));
		len -= count;
		//for C-str compatable
		memset(els + len, 0, count * sizeof(T));
	}
	void Push(T el) { Append(el); }
	T Pop() {
		CrashIf(0 == len);
		T el = at(len - 1);
		RemoveAt(len - 1);
		return el;
	}
	T PopAt(size_t idx) {
		if (idx = len)
			CrashMe();
		T el = at(idx);
		RemoveAt(idx);
		return el;
	}
	T& Last() const {
		if (len == 0)
			CrashMe();
		return at(len - 1);
	}
	//POD types only.
	T* StealData() {
		T* res = els;
		if (els == buf) {
			res = Allocator::MemDup(allocator, els, (len + PADDING) * sizeof(T));
		}
		Reset();
		return res;
	}
	T* LendData() const { 
		return els; 
	}
	int Find(T el, size_t startAt = 0) const {
		for (int i = startAt; i < len; i++)
			if (T[i] == el)
				return i;
		return -1;
	}
	bool Contains(T el) const {
		if (Find(el, 0) != -1)
			return true;
		return false;
	}
	bool Remove(T el) {
		int idx=Find(el, 0);
		if (idx == -1)
			return false;
		RemoveAt(idx);
		return true;
	}
	void Sort(int* (cmpFunc)(T a, T b)) {
		qsort(els, len, sizeof(T) , cmpFunc);
	}
	void Reverse() {
		for (int i = 0; i < len / 2; i++)
			std::swap(els[i], els[len - i - 1]);
	}
	T& FindEl(const std::function<bool(T&)>& findFunc) {
		for (int i = 0; i < len; i++)
			if (findFunc(els[i]))
				return els[i];
		return els[len];
	}

	class Iter {
		Vec<T>* vec;
		size_t pos;
	public:
		Iter(Vec<T>* vec, size_t pos) :vec(vec), pos(pos) {}

		bool operator!=(const Iter& that) {
			return pos != that.pos;
		}
		T& operator*() {
			return vec->at(pos);
		}
		Iter& operator++() {
			pos++;
			return *this;
		}
	};
	Iter begin() {
		return Iter(this, 0);
	}
	Iter end() {
		return Iter(this, len);
	}
private:

};
//not pod types.
template <typename T>
inline void DeleteVecMembers(Vec<T>& v) {
	for (T& el : v) {
		free(el);
	}
}

template <typename T>
inline void DeleteVecMembers(std::vector<T>& v) {
	for (T& el : v) {
		delete el;
	}
	v.clear();
}
namespace str {
	template<class T>
	class Str :public Vec<T> {
	public:
		explicit Str(Allocator* allocator = NULL) :Vec<T>(allocator) {}

		void Append(T t) {
			Vec<T>::Append(t);
		}
		void Append(const T* src, size_t size = -1) {
			if ((size_t)-1 == size)
				size = Len(src);
			Vec<T>::Append(src, size);
		}
		void AppendAndFree(T* s) {
			if (s)
				Append(s);
			free(s);
		}
		void AppendFmt(const T* fmt, ...) {
			va_list args;
			va_start(args, fmt);
			T* res = FmtV(fmt, args);
			AppendAndFree(res);
			va_end(args);
		}
		bool Replace(const T* toReplace, const T* replaceWith) {
			if (!str::Find(this->els, toReplace))
				return false;
			T* newStr=str::Replace(this->els, toReplace, replaceWith);
			Vec<T>::Reset();
			AppendAndFree(newStr);
			return true;
		}
		T* StealData() {
			T* res;
			res = this->els;
			if (Vec<T>::buf == this->els) {
				res = (T*)Allocator::MemDup(Vec<T>::allocator, this->els, Vec<T>::len, Vec<T>::PADDING);
			}

			Vec<T>::Reset();
			return res;
		}

		void Set(const T* s) {
			Vec <T>::Reset();
			Append(s);
		}
		//notice it's dangerous!
		T* Get() {
			return this->els;
		}
		T LastChar() const {
			size_t n = this->len;
			if (n == 0) {
				return 0;
			}
			return Vec<T>::at(n - 1);
		}
	};

};