#pragma once

#include <algorithm>
#include <string>
#include "estl/intrusive_ptr.h"

namespace reindexer {

using std::string;

typedef const string const_string;

class base_key_string : public string {
public:
	template <typename... Args>
	base_key_string(Args &&... args) : string(args...) {
		bind();
	}

	template <typename... Args>
	void assign(Args &&... args) {
		const_string::assign(args...);
		bind();
	}
	static ptrdiff_t export_hdr_offset() {
		static base_key_string sample;
		return ptrdiff_t(reinterpret_cast<const char *>(&sample.export_hdr_) - reinterpret_cast<const char *>(&sample));
	}

	// delete all modification methods - to be sure, that base_key_string is mutable, and export will not invalidate after construction
	iterator begin() = delete;
	iterator end() = delete;
	char &operator[](int) = delete;
	template <typename... Args>
	void insert(Args &&... args) = delete;
	template <typename... Args>
	void append(Args &&... args) = delete;
	template <typename... Args>
	void copy(Args &&... args) = delete;
	template <typename... Args>
	void replace(Args &&... args) = delete;
	void push_back(char c) = delete;
	template <typename... Args>
	void erase(Args &&... args) = delete;
	template <typename... Args>
	void reserve(Args &&... args) = delete;
	template <typename... Args>
	void resize(Args &&... args) = delete;
	void at(int) = delete;
	void shrink_to_fit() = delete;
	void clear() = delete;

protected:
	void bind() {
		export_hdr_.cstr = string::c_str();
		export_hdr_.len = length();
	}

	struct export_hdr {
		const void *cstr;
		int len;
	} export_hdr_;
};

typedef intrusive_ptr<intrusive_atomic_rc_wrapper<base_key_string>> key_string;

template <typename... Args>
key_string make_key_string(Args &&... args) {
	return make_intrusive<intrusive_atomic_rc_wrapper<base_key_string>>(args...);
}

// Unckecked cast to derived class!
// It assumes, that all strings in payload are intrusive_ptr and stored with intrusive_atomic_rc_wrapper
inline void key_string_add_ref(string *str) {
	intrusive_ptr_add_ref(reinterpret_cast<intrusive_atomic_rc_wrapper<base_key_string> *>(str));
}
inline void key_string_release(string *str) {
	intrusive_ptr_release(reinterpret_cast<intrusive_atomic_rc_wrapper<base_key_string> *>(str));
}

}  // namespace reindexer
namespace std {
template <>
struct hash<reindexer::base_key_string> {
public:
	size_t operator()(const reindexer::base_key_string &obj) const { return hash<std::string>()(obj); }
};
}  // namespace std
