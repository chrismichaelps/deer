---
Language: C++
Version: C++20 (ISO/IEC 14882:2020)
Fidelity: 100%
State_ID: BigInt(0x1)
LSP_Discovery_Root: "@root"
Grammar_Lock: "@root/hashes/grammar/cpp.hash.md"
---
## @SDK_Discovery_Map
/** @Ref: cppreference.com/w/cpp/headers.html */

/** @Library: Language Support */
- <cstddef> - Standard macros and typedefs (size_t, nullptr_t)
- <cstdint> - Fixed-width integer types (C++11)
- <limits> - Query properties of arithmetic types
- <new> - Low-level memory management utilities
- <typeinfo> - Runtime type information utilities
- <source_location> - Source code location (C++20)
- <compare> - Three-way comparison operator (C++20)
- <coroutine> - Coroutine support (C++20)
- <initializer_list> - std::initializer_list (C++11)

/** @Library: Diagnostics */
- <stdexcept> - Standard exception types (runtime_error, logic_error)
- <cassert> - Conditionally compiled macro (assert)
- <system_error> - std::error_code (C++11)

/** @Library: Memory Management */
- <memory> - Smart pointers (shared_ptr, unique_ptr, make_shared, make_unique)
- <memory_resource> - Polymorphic allocators (C++17)

/** @Library: Metaprogramming */
- <type_traits> - Compile-time type information (C++11)
- <ratio> - Compile-time rational arithmetic (C++11)

/** @Library: General Utilities */
- <functional> - Function objects, bind operations
- <tuple> - std::tuple (C++11)
- <variant> - std::variant (C++17)
- <optional> - std::optional (C++17)
- <any> - std::any (C++17)
- <expected> - std::expected (C++23)
- <utility> - Various utility components (move, swap, pair)
- <bitset> - std::bitset

/** @Library: Containers */
- <vector> - std::vector
- <array> - std::array (C++11)
- <list> - std::list
- <deque> - std::deque
- <map> - std::map, std::multimap
- <unordered_map> - std::unordered_map (C++11)
- <unordered_set> - std::unordered_set (C++11)
- <set> - std::set, std::multiset
- <queue> - std::queue, std::priority_queue
- <stack> - std::stack
- <span> - std::span (C++20)

/** @Library: Strings */
- <string> - std::basic_string
- <string_view> - std::basic_string_view (C++17)

/** @Library: Input/Output */
- <iostream> - Standard stream objects (cout, cin, cerr)
- <fstream> - std::basic_fstream, std::basic_ifstream, std::basic_ofstream
- <sstream> - std::basic_stringstream
- <filesystem> - std::filesystem::path (C++17)
- <ios> - std::ios_base, std::basic_ios
- <iosfwd> - Forward declarations of I/O classes

/** @Library: Concurrency */
- <atomic> - Atomic operations (C++11)
- <thread> - std::thread (C++11)
- <mutex> - Mutual exclusion primitives (C++11)
- <future> - Primitives for asynchronous computations (C++11)
- <barrier> - Barriers (C++20)
- <latch> - Latches (C++20)
- <semaphore> - Semaphores (C++20)
- <stop_token> - Stop tokens (C++20)

/** @Library: Ranges */
- <ranges> - Range access and adaptors (C++20)
- <concepts> - Fundamental library concepts (C++20)

/** @Library: Algorithms */
- <algorithm> - Algorithms that operate on ranges
- <numeric> - Numeric operations on ranges

/** @Library: Numerics */
- <cmath> - Common mathematics functions
- <complex> - Complex number type
- <random> - Random number generators (C++11)
- <numbers> - Math constants (C++20)

/** @Library: Time */
- <chrono> - C++ time utilities (C++11)

/** @Library: Text Processing */
- <regex> - Regular expression processing (C++11)
- <format> - Formatting library (C++20)

## @SDK_Imports
/** @Core: Memory */
- std::shared_ptr<T>, std::unique_ptr<T>, std::make_shared<T>(args...), std::make_unique<T>(args...)
- std::static_pointer_cast<T>, std::dynamic_pointer_cast<T>
- std::allocate_shared<T>(alloc, args...)

/** @Core: Functional */
- std::function<ReturnType(Args...)>
- std::bind(callable, args...)
- std::placeholders::_1, std::placeholders::_2

/** @Core: Containers */
- std::vector<T>, std::map<K,V>, std::unordered_map<K,V>
- std::array<T,N>, std::span<T>
- std::pair<K,V>, std::tuple<Ts...>

/** @Core: Optional/Variant */
- std::optional<T>, std::nullopt
- std::variant<Ts...>, std::monostate, std::get<T>, std::visit

/** @Core: Strings */
- std::string, std::string_view
- std::to_string, std::stoi, std::stod

/** @Core: I/O */
- std::cout, std::cin, std::cerr, std::clog
- std::endl, std::flush
- std::ifstream, std::ofstream, std::fstream
- std::ostringstream, std::istringstream

/** @Core: Filesystem */
- std::filesystem::path, std::filesystem::exists(), std::filesystem::create_directory()

/** @Core: Time */
- std::chrono::steady_clock, std::chrono::system_clock
- std::chrono::duration, std::chrono::time_point

/** @Core: Thread */
- std::thread, std::mutex, std::lock_guard, std::unique_lock
- std::atomic<T>, std::atomic_flag

/** @Core: Concepts (C++20) */
- std::same_as<T,U>, std::convertible_to<T,U>
- std::copyable, std::movable, std::constructible_from
- std::invocable, std::regular_invocable

/** @Core: Ranges (C++20) */
- std::ranges::range, std::ranges::view
- std::ranges::begin, std::ranges::end

/** @Core: Error Handling */
- std::runtime_error, std::logic_error, std::invalid_argument
- std::exception, std::terminate, std::uncaught_exceptions()

## @Core_Primitives
/** @Type: Fundamental */
- void, bool, char, int, float, double
- size_t, ptrdiff_t, nullptr_t
- auto, decltype(expression)

/** @Type: Qualifiers */
- const, volatile, & (lvalue reference), && (rvalue reference)
- const&, const&&, volatile&, volatile&&

/** @Type: cv-qualifiers on members */
- mutable

/** @Type: Storage */
- static, extern, register (deprecated), thread_local
- inline, constexpr, consteval (C++20), constinit (C++20)

/** @Type: Access */
- public, private, protected

/** @Type: Inheritance */
- virtual, override, final
- =0 (pure virtual), = default, = delete

/** @Type: Smart Pointers */
- std::shared_ptr<T>, std::unique_ptr<T>
- std::weak_ptr<T>

/** @Type: Template */
- template <typename T>, template <class T>
- template <typename T = Default>, template <typename... Ts>
- requires (C++20)

/** @Type: Concept (C++20) */
- concept Name = constraint

/** @Type: Lambda */
- [capture](params) -> ret { body }
- [=, &var, this, *this]

/** @Type: Coroutine (C++20) */
- co_await, co_yield, co_return

/** @Type: Attributes */
- [[nodiscard]], [[maybe_unused]], [[deprecated]]
- [[likely]], [[unlikely]], [[no_unique_address]]

## @Architectural_Laws
/** @Export_Law */
- Header-only modules use #pragma once
- structs/classes declared in .h with inline implementation
- Separate .cpp only for explicit instantiation avoidance
- Public API in headers, implementation details in anonymous namespace

/** @Transformation_Law */
- Use factory functions: make_shared<T>(), make_unique<T>()
- Use std::make_pair(), std::make_tuple()
- Avoid raw pointers for ownership (use smart pointers)
- Data conversion: Database Row -> Domain Object in Repository layer only

/** @Propagation_Law */
- Throw std::runtime_error for runtime errors
- Throw std::logic_error for programming errors
- Custom exception classes inherit from std::runtime_error
- Wrap sibling shard errors into service-level error; do not leak raw errors

/** @Namespace_Law */
- Nested namespaces: namespace deer { namespace config { } }
- Inline namespaces for versioning: namespace deer { inline namespace v1 { } }
- using declaration for specific symbols only

/** @Template_Law */
- Use concept constraints (C++20) instead of enable_if where possible
- Prefer template<concepts::Movable T> over template<typename T, typename = enable_if_t<...>>
- Use requires clauses for constraints

/** @Concurrency_Law */
- Prefer std::atomic for simple atomics
- Use std::mutex with std::lock_guard or std::unique_lock
- Avoid deadlock: acquire locks in consistent order
- Use std::jthread (C++20) for cooperative cancellation

## @Syntax_Rules
/** @Naming */
- PascalCase: Classes, Structs, Enums, Concepts, Type aliases
- camelCase: functions, methods, variables, function templates
- SCREAMING_CAPS: Constants, constexpr, enum values, macro names
- snake_case: file_names, directory_names

/** @Namespace */
- Lowercase with underscores: deer::config, deer::ops
- Prefer nested namespaces over flat

/** @Indentation */
- 4 spaces (no tabs)
- Open brace on same line for namespaces/classes
- Break before binary operators (long lines)

/** @Header */
- #pragma once at top
- Include guards allowed but #pragma once preferred
- Order: Related header, Standard library, External libraries, Project headers

/** @Template */
- Declaration: template<typename T> or template<class T>
- Constraints: template<std::movable T> or template<typename T> requires std::movable<T>
- Use using for type aliases: using Ptr = std::shared_ptr<T>;

/** @Control */
- if (condition) { } else { }
- for (auto& item : container) { }
- while (condition) { }
- switch (value) { case X: break; default: break; }

/** @C++20_Specific */
- Use std::format("{}", value) instead of std::cout <<
- Use std::span instead of raw pointer + size
- Use [[nodiscard]] for functions that must be called
- Use std::jthread instead of std::thread for RAII

/** @Auto */
- Prefer auto for type deduction
- Use auto& for non-copying range-based for
- Use auto&& for perfect forwarding in lambdas

## @Prohibited_Patterns
/** @Memory */
- No raw new/delete (use smart pointers)
- No raw pointer ownership (use unique_ptr/shared_ptr)
- No malloc/free (use new/delete or smart pointers)

/** @Legacy */
- No var keyword (use auto)
- No C-style casts (use static_cast, reinterpret_cast)
- No typedef (use using)
- No using namespace in headers

/** @Style */
- No hungarian notation (m_, _ prefix allowed but discouraged)
- No public member variables (use properties or getters)
- No goto (use proper control flow)
- No std::endl (use \n or std::flush)

/** @Concurrency */
- No std::mutex as member without proper RAII
- No raw std::thread (use std::jthread C++20)
- No busy-wait loops (use condition_variable or atomic)

/** @Error_Handling */
- Do not catch(...) except at top level
- Do not silently swallow exceptions
- Do not use errno directly (use std::error_code)

## @Standard_Library_Signatures
/** @Smart_Pointers */
- std::make_shared<T>(args...) -> std::shared_ptr<T>
- std::make_unique<T>(args...) -> std::unique_ptr<T>
- std::shared_ptr<T>::get() -> T*
- std::shared_ptr<T>::use_count() -> long
- std::unique_ptr<T>::release() -> T*

/** @Function */
- std::function<R(Args...)>
- std::bind(&Class::method, &obj, args...) -> std::function<R()>
- std::invoke(callable, args...) -> R (C++17)

/** @Containers */
- std::vector<T>::push_back(const T&)
- std::vector<T>::emplace_back(args...)
- std::map<K,V>::operator[](const K&) -> V&
- std::unordered_map<K,V>::find(const K&) -> iterator
- container.begin(), container.end()
- container.empty(), container.size()

/** @Optional */
- std::optional<T>::has_value() -> bool
- std::optional<T>::value() -> T&
- std::optional<T>::value_or(T) -> T

/** @Variant */
- std::variant<Ts...>::index() -> size_t
- std::get<T>(variant) -> T&
- std::visit(visitor, variant)

/** @String */
- std::string::c_str() -> const char*
- std::string::size() -> size_t
- std::string::empty() -> bool
- std::string::append(const std::string&)

/** @Filesystem */
- std::filesystem::path::native() -> native_string
- std::filesystem::exists(path) -> bool
- std::filesystem::create_directories(path) -> bool
- std::filesystem::remove(path) -> bool

/** @Thread */
- std::thread::joinable() -> bool
- std::thread::join(), std::thread::detach()
- std::mutex::lock(), std::mutex::unlock()
- std::lock_guard<Mutex>(mutex)
- std::atomic<T>::load(), std::atomic<T>::store()

/** @Time */
- std::chrono::steady_clock::now() -> time_point
- std::chrono::duration_cast<Duration>(duration)
- std::this_thread::sleep_for(Duration)

/** @Concepts (C++20) */
- std::same_as<T, U>
- std::derived_from<T, U>
- std::convertible_to<T, U>
- std::invocable<F, Args...>
- std::movable<T>

/** @Ranges (C++20) */
- std::ranges::begin(range) -> iterator
- std::ranges::end(range) -> sentinel
- std::ranges::range<T>
- std::ranges::view<T>