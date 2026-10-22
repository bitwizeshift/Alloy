package includes

// standard holds the headers of the C++ standard library, and of the C
// standard library in both their C and C++ spellings.
var standard = map[string]bool{
	// C++ library headers
	"algorithm": true, "any": true, "array": true, "atomic": true,
	"barrier": true, "bit": true, "bitset": true, "charconv": true,
	"chrono": true, "codecvt": true, "compare": true, "complex": true,
	"concepts": true, "condition_variable": true, "contracts": true,
	"coroutine": true, "debugging": true, "deque": true, "exception": true,
	"execution": true, "expected": true, "filesystem": true, "flat_map": true,
	"flat_set": true, "format": true, "forward_list": true, "fstream": true,
	"functional": true, "future": true, "generator": true,
	"hazard_pointer": true, "hive": true, "initializer_list": true,
	"inplace_vector": true, "iomanip": true, "ios": true, "iosfwd": true,
	"iostream": true, "istream": true, "iterator": true, "latch": true,
	"limits": true, "linalg": true, "list": true, "locale": true, "map": true,
	"mdspan": true, "memory": true, "memory_resource": true, "meta": true,
	"mutex": true, "new": true, "numbers": true, "numeric": true,
	"optional": true, "ostream": true, "print": true, "queue": true,
	"random": true, "ranges": true, "ratio": true, "rcu": true, "regex": true,
	"scoped_allocator": true, "semaphore": true, "set": true,
	"shared_mutex": true, "simd": true, "source_location": true, "span": true,
	"spanstream": true, "sstream": true, "stack": true, "stacktrace": true,
	"stdexcept": true, "stdfloat": true, "stop_token": true, "streambuf": true,
	"string": true, "string_view": true, "strstream": true,
	"syncstream": true, "system_error": true, "text_encoding": true,
	"thread": true, "tuple": true, "type_traits": true, "typeindex": true,
	"typeinfo": true, "unordered_map": true, "unordered_set": true,
	"utility": true, "valarray": true, "variant": true, "vector": true,
	"version": true,

	// C library headers, as included from C++
	"cassert": true, "cctype": true, "cerrno": true, "cfenv": true,
	"cfloat": true, "cinttypes": true, "climits": true, "clocale": true,
	"cmath": true, "csetjmp": true, "csignal": true, "cstdarg": true,
	"cstddef": true, "cstdint": true, "cstdio": true, "cstdlib": true,
	"cstring": true, "ctime": true, "cuchar": true, "cwchar": true,
	"cwctype": true,

	// C library headers
	"assert.h": true, "complex.h": true, "ctype.h": true, "errno.h": true,
	"fenv.h": true, "float.h": true, "inttypes.h": true, "iso646.h": true,
	"limits.h": true, "locale.h": true, "math.h": true, "setjmp.h": true,
	"signal.h": true, "stdalign.h": true, "stdarg.h": true,
	"stdatomic.h": true, "stdbit.h": true, "stdbool.h": true,
	"stdckdint.h": true, "stddef.h": true, "stdint.h": true, "stdio.h": true,
	"stdlib.h": true, "stdnoreturn.h": true, "string.h": true,
	"tgmath.h": true, "threads.h": true, "time.h": true, "uchar.h": true,
	"wchar.h": true, "wctype.h": true,
}
