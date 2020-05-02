#pragma once

#define basicsys_delete(x)	\
	if ((x) != NULL) {delete (x); (x) = NULL;}

#define basicsys_delete_array(x)	\
	if ((x) != NULL) {delete[] (x); (x) = NULL;}

#define basicsys_delete_vector_ptr(x)	\
	for (int i = 0; i < (int)x.size(); ++i) {\
	basicsys_delete(x[i]);\
	}

#define basicsys_delete_vector_array_ptr(x)	\
	for (int i = 0; i < (int)x.size(); ++i) {\
	basicsys_delete_array(x[i]);\
	}

#define basicsys_free(x)	\
	if ((x) != NULL) {free((x)); (x) = NULL;}


#define basicsys_disable_copy(class_name)	\
protected:	\
	void operator=(const class_name& other) {}

#define basicsys_disable_construction(class_name)	\
protected:	\
	class_name() {}	\



#define basicsys_class_name_method(class_description)	\
	static const string& class_name() { \
		static string name = #class_description; \
		return name; \
	}	\
	virtual string object_class_name() const { \
		return #class_description; \
	}

namespace basicsys {

	typedef short              i16;
	typedef int                i32;
	typedef long long          i64;
	typedef unsigned char      u8;
	typedef unsigned short     u16;
	typedef unsigned int       u32;
	typedef unsigned long long u64;

	typedef char				i8;
	typedef wchar_t				c16;

	typedef float				f32;
	typedef double				f64;

	/** Instead of bool, because vector<bool> is not an normal vector!
	*/
	struct b8 {

		b8(bool value) {
			m_data = value ? 1 : 0;
		}

		b8(i32 data = 0)
		: m_data(data) {

		}

		b8 operator !() const {
			return m_data ? b8(0) : b8(1);
		}

		operator u8() const {
			return m_data;
		}

		u8 m_data;
	};

	static const b8 sys_true = 1;
	static const b8 sys_false = 0;
}


//#define basicsys_enable_mkl


#ifndef _WIN32

static int fopen_s(FILE ** fp, const char * path, const char * flag) {
	*fp = fopen(path, flag);

	if (*flag != NULL) {
		return 0;
	}

	return -1;
}

#define _access access
#define _isnan isnan
#define _finite finite

static void strncpy_s(char* dst, basicsys::i32 dist_size, const char* src, basicsys::i32 src_pos) {
	strncpy(dst, src, src_pos);
}

static basicsys::i64 _atoi64(const char* val) {
	return atoll(val);
}

#endif