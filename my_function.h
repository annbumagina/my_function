#include <memory>
#include <cstddef>
#include <iostream>

const size_t SIZE = 16;

template <typename T>
struct my_function;

template <typename Ret, typename ... Args>
struct my_function <Ret (Args ...)> {
	my_function() noexcept : fptr(nullptr), small(0) {}
	~my_function() {
		if (small) {
			((f_ptr_base *) buf)->~f_ptr_base();
		}
		if (!small && fptr != nullptr) {
			delete fptr;
		}
	}
	my_function(std::nullptr_t) noexcept : fptr(nullptr), small(0) {}
	template <typename F>
	my_function(F f) : fptr(nullptr), small(0) {
		if (sizeof(f_ptr<F>) <= SIZE) {
			std::cout << sizeof(f_ptr<F>) << ' ' << sizeof(F) << '\n';
			new (buf) f_ptr<F>(std::move(f));
			small = 1;
			std::cout << "small" << std::endl;
		} else {
			fptr = new f_ptr<F>(std::move(f));
			small = 0;
			std::cout << "big" << std::endl;
		}
	}
	my_function(my_function const& other) : fptr(nullptr), small(0) {
		if (other.small) {
			((f_ptr_base *) other.buf)->placement_copy_f(buf);
			small = 1;
		} else {
			fptr = other.fptr->copy_f();
			small = 0;
		}
	}
	my_function(my_function && other) noexcept : fptr(nullptr), small(0) {
		if (other.small) {
			((f_ptr_base *) other.buf)->placement_move_f(buf);
		} else {
			fptr = other.fptr;
		}
		other.fptr = nullptr;
		small = other.small;
		other.small = 0;
	}
	my_function& operator=(my_function const& other) {
		my_function tmp(other);
		swap(tmp);
		return *this;
	}
	my_function& operator=(my_function && other) noexcept {
		if (small) {
			if (other.small) {
				((f_ptr_base *) other.buf)->placement_move_f(buf);
			} else {
				fptr = other.fptr;
			}
			other.fptr = nullptr;
		} else {
			f_ptr_base *tmp = fptr;
			if (other.small) {
				((f_ptr_base *) other.buf)->placement_move_f(buf);
			} else {
				fptr = other.fptr;
			}
			other.fptr = tmp;
		}
		small = other.small;
		other.small = 0;
		return *this;
	}
	void swap(my_function &other) noexcept {
		std::swap(*this, other);
	}
	explicit operator bool() noexcept {
		if (!small && fptr == nullptr)
			return 0;
		else
			return 1;
	}
	Ret operator()(Args ... args) const {
		if (small) {
			return ((f_ptr_base *) buf)->invoke(std::forward<Args>(args) ...);
		}
		return fptr->invoke(std::forward<Args>(args) ...);
	}


	struct f_ptr_base {
		f_ptr_base() {}
		virtual Ret invoke(Args ...) = 0;
		virtual f_ptr_base *copy_f() = 0;
		virtual f_ptr_base *placement_copy_f(char *) = 0;
		virtual f_ptr_base *placement_move_f(char *) = 0;
	};

	template <typename F>
	struct f_ptr : public f_ptr_base {
		f_ptr(F f) : f_ptr_base(), f(f) {}
		Ret invoke(Args ... args) {
			return f(std::forward<Args>(args) ...);
		}
		f_ptr_base *copy_f() {
			return new f_ptr<F>(f);
		}
		f_ptr_base *placement_copy_f(char *bf) {
			new (bf) f_ptr<F>(f);
		}
		f_ptr_base *placement_move_f(char *bf) {
			new (bf) f_ptr<F>(std::move(f));
		}
		F f;
	};

	union {
		f_ptr_base *fptr;
		char buf[SIZE];
	};
	bool small;
};
