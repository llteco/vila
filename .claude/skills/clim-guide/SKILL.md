---
name: clim-guide
description: Guidance for using the clim (C++ Library for Infrastructure and Math) header-only utility library. Use this skill when working with any clim component: argparse for command-line parsing, string utilities (str_split, strip, const_string, str_replace), container utilities (bounding_box, ringbuffer, array_arithmetic), path handling, filters (Kalman, alpha-beta), hash functions (CityHash, MurmurHash), math utilities (quaternion, numerical), OS utilities, reflection, vt (vector tensor operations), base64 encoding, or zip utilities.
type: skill
---

# CLIM Library Usage Guide

CLIM is a header-only C++ utility library for computer vision and general infrastructure. All components are under `clim/` and can be included directly. Dependencies are minimal; most headers are standalone.

## Bazel Dependencies

```bazel
deps = ["//clim:<target>"]
```

Available targets: `argparse`, `base64`, `container`, `filter`, `hash`, `math`, `os`, `path`, `reflection`, `string`, `vt`, `zip`, `clim` ( umbrella target for all).

---

## argparse - Command-Line Argument Parsing

**Header:** `clim/argparse.h`
**Depends on:** `//clim:string`

Python-style argument parser supporting position args, keyword args, and flags.

```cpp
#include "clim/argparse.h"

auto parser = ArgumentParser("myprog");
parser.AddArgument("--foo").WithDefault(100).Required().WithHelp("Foo value");
parser.AddArgument("--bar").StoreTrue().WithHelp("Enable bar");
parser.AddArgument("input").WithHelp("Input file");  // position arg

auto args = parser.Parse(argc, argv);
int64_t foo = args["foo"].AsLong();  // or: args["foo"]
bool bar = args["bar"];              // implicit bool conversion
std::string input = args["input"].AsStr();

// Parse only known args, return unknown as map
auto [known, unknown] = parser.ParseKnown(argc, argv);

// Parse from dict (for configuration)
std::map<std::string, std::string> dict = {"foo", "200"};
auto args = parser.ParseDict(dict);
```

**Argument modifiers:**
- `WithDefault(int64_t|int32_t|float|double|std::string)` - set default value
- `Required()` - mark as required
- `StoreTrue()` / `StoreFalse()` - boolean flags
- `Nargs(uint64_t)` - number of values (use '*' or '+' for variable)
- `WithHelp(std::string_view)` - help message

---

## string - String Utilities

**Headers:** `clim/str_split.h`, `clim/strip.h`, `clim/str_replace.h`, `clim/const_string.h`
**Target:** `//clim:string`

### StrSplit / StrSplitStringView

```cpp
#include "clim/str_split.h"

// Split by character (returns std::vector<std::string>)
auto parts = StrSplit("a,b,c", ',');  // ["a", "b", "c"]

// Split by string (returns string_view for efficiency)
auto parts = StrSplitStringView("a::b::c", "::");  // string_view vector
auto parts = StrSplitStringView("a,b,c", ',');     // char version, faster
```

### Strip

```cpp
#include "clim/strip.h"

auto s = Strip("  hello  ");       // "hello" (strips spaces by default)
auto s = Strip("__hello__", '_');  // "hello" (custom char)
```

### StrReplace

```cpp
#include "clim/str_replace.h"

auto s = StrReplace("hello world", "world", "there");  // "hello there"
std::string mutable_str = "abcabc";
StrReplaceInplace(mutable_str, "abc", "x");  // modifies in-place: "xx"
```

### ConstString (Compile-Time String)

```cpp
#include "clim/const_string.h"

constexpr auto s = ConstString("abc");
s.Size();                      // 4 (includes null terminator)
s.Data();                      // "abc"
s[1];                          // 'b'
s.Substr<1, 2>();              // ConstString("bc")
s.Find('c');                   // 2
s.ReverseFind('c');            // 2

// Useful with macros
ConstString(__FUNCTION__).Find(':');  // parse function name at compile-time
```

---

## container - Bounding Boxes & Ring Buffer

**Headers:** `clim/bounding_box.h`, `clim/ringbuffer.h`, `clim/array_arithmetic.h`, `clim/rect.h`
**Target:** `//clim:container`

### BoundingBox

Templated bounding box with multiple representations:

```cpp
#include "clim/bounding_box.h"

// BoxType::xyxy - [left, top, right, bottom]
// BoxType::xywh - [left, top, width, height]
// BoxType::cxywh - [center_x, center_y, width, height]

BoundingBox<BoxType::xyxy, double> box(10, 20, 100, 200);
auto xywh = box.ToXYWH();    // convert representation
auto cxywh = box.ToCXYWH();  // convert to center format

box.Area();                  // area of box
box.Width();                 // width
box.Height();                // height
box.Intersect(other);        // intersection area (other must be xyxy)
box.Union(other);            // union area

// Relative/absolute coordinate conversion
auto rel = box.ToRelative(img_width, img_height);
auto abs = rel.ToAbsolute(img_width, img_height);
```

### RingBuffer

Simple FIFO ring buffer (not thread-safe):

```cpp
#include "clim/ringbuffer.h"

RingBuffer<int> buffer(10);  // capacity 10
buffer.Push(1);
buffer.Push(2);
buffer.Peak();    // 1 (oldest element, doesn't remove)
buffer.Pop();     // 1 (removes oldest)
buffer.Empty();   // check if empty
buffer.Capacity(); // 10
```

### array_arithmetic

Operator overloads for `std::array`:

```cpp
#include "clim/array_arithmetic.h"

std::array<double, 4> a = {1, 2, 3, 4};
std::array<double, 4> b = {5, 6, 7, 8};

auto c = a + b;       // element-wise add
auto d = a * 2.0;     // scalar multiply
auto e = a / b;       // element-wise divide
a += b;               // in-place
abs(a);               // element-wise abs
clip(a, 0.0, 10.0);   // element-wise clip
l1(a);                // L1 norm
l2(a);                // L2 norm
```

---

## filter - Kalman & Alpha-Beta Filters

**Headers:** `clim/kalman_filter.h`, `clim/alpha_beta_filter.h`
**Target:** `//clim:filter` (depends on `//clim:container`)

### AlphaBetaFilter

Simple state estimator for smoothing:

```cpp
#include "clim/alpha_beta_filter.h"

AlphaBetaFilter<4, double> filter;  // 4-element array filter
std::array<double, 4> measurement = {1.0, 2.0, 3.0, 4.0};

// Filter with alpha=0.5, beta=0.1, dt=0.1
auto smoothed = filter(measurement, 0.5, 0.1, 0.1);
auto predicted = filter.Predict(0.1);  // predict next state
auto velocity = filter.Velocity();     // current velocity estimate
filter.Reset();                        // reset state and velocity
```

### KalmanFilter

Full Kalman filter with state and covariance:

```cpp
#include "clim/kalman_filter.h"

std::array<double, 4> initial = {0, 0, 0, 0};
std::array<double, 4> variance = {1, 1, 1, 1};
KalmanFilter<4, double> kf(initial, variance);

auto priori = kf.Priori();  // predicted state

// Posteriori update with measurement, dt, measurement noise R, process noise Q
std::array<double, 4> measure = {1.1, 2.2, 3.3, 4.4};
std::array<std::array<double, 4>, 4> Q = {...};  // process noise per element
std::array<double, 4> R = {0.1, 0.1, 0.1, 0.1};   // measurement noise
auto posteriori = kf.Posteriori(measure, 0.1, R, Q);
```

---

## hash - CityHash & MurmurHash

**Headers:** `clim/hasher/hash.h`, `clim/hasher/city.h`, `clim/hasher/murmur3.h`
**Target:** `//clim:hash`

```cpp
#include "clim/hasher/hash.h"

MixStringHash hasher;
uint64_t h = hasher("some_string");  // CityHash64 for len>=16, std::hash otherwise

// Direct usage
#include "clim/hasher/city.h"
uint64_t h = CityHash64(data, len);
uint64_t h = CityHash64WithSeed(data, len, seed);

#include "clim/hasher/murmur3.h"
uint32_t h = Murmur3_32(data, len, seed);
```

---

## math - Quaternion & Numerical Utilities

**Headers:** `clim/quat.h`, `clim/numerical.h`
**Target:** `//clim:math` (depends on `//clim:traits`)

### Quaternion

3D rotation representation:

```cpp
#include "clim/quat.h"

Quaternion<double> q;                    // identity quaternion
Quaternion<double> q(0.1, 0.2, 0.3, 1.0); // from x,y,z,w
Quaternion<double> q(pitch, yaw, roll, EulerType::xyz);  // from Euler angles
Degree<double> pitch(45), yaw(30), roll(0);
Quaternion<double> q(pitch, yaw, roll);  // from degrees

q.W(), q.X(), q.Y(), q.Z();              // components
q.Normalize();                           // normalized quaternion
q.Norm();                                // L2 norm
q.Magnitude();                           // rotation angle
q.Conj();                                // conjugate

auto [p, y, r] = q.AsEuler(EulerType::zyx);  // to Euler angles

// Apply rotation to 3D vector
auto [x', y', z'] = q.Apply(1.0, 0.0, 0.0);  // rotate x-axis

Quaternion_d q1, q2;
auto result = q1 * q2;  // quaternion multiplication (compose rotations)
```

### numerical

Math constants and utilities:

```cpp
#include "clim/numerical.h"

numbers::pi;              // 3.14159...
numbers::e;               // 2.71828...
numbers::pi_v<float>;     // templated pi

round_div(7, 4);          // 2 (rounded up integer division)
align_floor(7, 4);        // 4
align_ceil(7, 4);         // 8
align(7, 4);              // 8

clip(x, -10.0, 10.0);     // clamp to range
deg2rad(90.0);            // π/2
rad2deg(numbers::pi);     // 180.0

numeric_div<double>(7, 3);  // safe division (avoids div-by-zero)
```

---

## os - Operating System Utilities

**Headers:** `clim/os.h`, `clim/aligned_malloc.h`, `clim/barrier.h`
**Target:** `//clim:os`

### Environ

```cpp
#include "clim/os.h"

auto envs = Environ();  // std::map<std::string, std::string> of all env vars
```

### aligned_malloc

```cpp
#include "clim/aligned_malloc.h"

float* ptr = aligned_malloc<float*>(1024, 4096);  // 4KB aligned
aligned_free(ptr);

// Default template returns uint8_t*
auto ptr = aligned_malloc(1024);  // default 4KB alignment
```

### Barrier

Thread synchronization (from Eigen):

```cpp
#include "clim/barrier.h"

Barrier barrier(3);  // wait for 3 notifications
// thread 1
barrier.Notify();
// thread 2
barrier.Notify();
// thread 3
barrier.Notify();
barrier.Wait();      // blocks until all 3 Notify() called
barrier.WaitFor(std::chrono::seconds(5));  // timed wait
barrier.Notified();  // check if notified
```

---

## path - Cross-Platform Path Handling

**Headers:** `clim/path.h`, `clim/os_path.h`
**Target:** `//clim:path`

Python pathlib-style path class:

```cpp
#include "clim/path.h"

Path p("/home/user/file.txt");
p.Exists();            // bool
p.IsFile();            // bool
p.IsDir();             // bool
p.IsAbsolute();        // bool
p.Name();              // "file.txt"
p.Stem();              // "file"
p.Suffix();            // ".txt"
p.Parent();            // Path("/home/user")
p.Str();               // native string
p.AsPosix();           // forward slashes
p.AsUri();             // "file:///home/user/file.txt"

p.Absolute();          // absolute path
p.Resolve();           // canonical path (resolves symlinks)
Path::CWD();           // current working directory

p / "subdir";          // path concatenation
p.WithName("new.txt"); // change filename
p.WithSuffix(".csv");  // change extension
p.WithStem("data");    // change stem

p.Glob("*.txt");       // std::vector<Path> matching pattern
p.RGlob("*.txt");      // recursive glob
p.RelativeTo(Path("/home"));  // relative path

Path("/home/user").IsRelativeTo(Path("/home"));  // true
```

---

## reflection - File Path Macros

**Header:** `clim/reflect.h`
**Target:** `//clim:reflection` (depends on `//clim:string`)

```cpp
#include "clim/reflect.h"

FILESTEM("/path/to/file.cpp");   // "file" (basename without extension)
BASENAME("/path/to/file.cpp");   // "file.cpp" (basename)
```

---

## vt - Vector Tensor Operations

**Headers:** `clim/vt/vt.h`, `clim/vt/vt_gemm.h`, `clim/vt/vt_nn.h`
**Target:** `//clim:vt` (depends on `//clim:traits`)

Header-only tensor library for STL containers. Works with `std::vector`, `std::list`, etc.

### Basic Operations

```cpp
#include "clim/vt/vt.h"
using namespace vt;

std::vector<double> a = {1, 2, 3, 4};
std::vector<double> b = {5, 6, 7, 8};

Add(a, b);             // element-wise add
Sub(a, b);             // element-wise subtract
Mul(a, b);             // element-wise multiply
Div(a, b);             // element-wise divide
Mul(a, 2.0);           // scalar multiply

Abs(a);                // absolute values
ReduceSum(a);          // sum
ReduceMean(a);         // average
ReduceMax(a);          // max
ReduceMin(a);          // min
Median(a);             // median
Dot(a, b);             // dot product
PSNR(a, b);            // peak signal-to-noise ratio

PI(a);                 // product of all elements (Π)
PI(shape.begin(), shape.end());  // product of shape elements

Cast<std::vector<float>>(a);  // type conversion

Arange<std::vector<int>>(0, 10, 2);  // [0, 2, 4, 6, 8] (like numpy.arange)
Ones<std::vector<double>>(shape);    // tensor of ones
Zeros<std::vector<double>>(shape);   // tensor of zeros
RandomN<std::vector<double>>(100);   // random uniform
```

### Tensor Index Operations

```cpp
std::vector<int> shape = {2, 3, 4};  // 3D tensor
std::vector<int> index = {1, 2, 3};

// Convert N-D index to flat index
int flat = Index(index, shape);  // 1*3*4 + 2*4 + 3 = 23

// Convert flat index to N-D index
auto nd = RevIndex(23, shape);   // {1, 2, 3}

// Slice tensor (use -1 to keep all along axis)
auto sliced = SliceAny(data, shape, {-1, 1, 2});

// Crop tensor
auto cropped = Crop(data, shape, {0, 1, 0}, {2, 2, 4});

// Transpose
auto perm = {2, 1, 0};  // reverse dimensions
auto transposed = Transpose(data, shape, perm);

// Concatenate
auto combined = Concat(a, shape_a, b, shape_b, axis);

// Pad
auto padded = Pad(data, shape, {1, 1, 2, 2});  // pad before/after each dim
```

### GEMM (Matrix Multiplication)

```cpp
#include "clim/vt/vt_gemm.h"

std::vector<double> a = {...};  // shape [B, M, K]
std::vector<double> b = {...};  // shape [B, K, N]
std::vector<int> as = {1, 4, 3};
std::vector<int> bs = {1, 3, 5};

auto result = Gemm(a, as, b, bs);  // shape [B, M, N]

// With accumulation
auto c = Zeros<std::vector<double>>({1, 4, 5});
Gemm(a, as, b, bs, c, cs);  // c = a @ b + c

// Scaled Dot-Product Attention (SDPA)
auto attn = SDPA(query, qs, key, ks, value, vs, mask, ms, scale);

// Flash Attention (blocked algorithm)
auto attn = FlashAttention<64>(query, qs, key, ks, value, vs, mask, ms, scale);
```

### Neural Network Operations

```cpp
#include "clim/vt/vt_nn.h"

// 2D Convolution
auto conv = Conv(input, {IC, H, W}, weights, {OC, IC, K, K}, pad, groups, dilation, stride);

// 2D Transposed Convolution
auto deconv = Deconv(input, {IC, H, W}, weights, {IC, OC, K, K}, stride, pad, opad, groups, dilation);

// Pooling
auto pooled = MaxPool(data, shape, block_size);
auto pooled = AvgPool(data, shape, block_size);

// Broadcasting
auto result = BroadcastAdd(a, as, b, bs);  // NumPy-style broadcasting
auto result = BroadcastMul(a, as, b, bs);

// Activations
auto activated = Relu(data);
auto activated = Sigmoid(data);
auto activated = Tanh(data);
auto activated = Softmax(data, shape, dim);
```

---

## base64 - Base64 Encoding/Decoding

**Header:** `clim/base64.h`
**Target:** `//clim:base64`

```cpp
#include "clim/base64.h"

// Encode
std::string encoded = base64::encode(data_ptr, length);

// Decode
std::vector<uint8_t> decoded = base64::decode(encoded_str, encoded_len);
```

---

## zip - Container Zip Utility

**Header:** `clim/zip.h`
**Target:** `//clim:zip`

Zip two containers or tuples:

```cpp
#include "clim/zip.h"

std::vector<int> a = {1, 2, 3};
std::vector<char> b = {'a', 'b', 'c'};
auto pairs = zip(a, b);  // std::vector<std::pair<int, char>>: [(1,'a'), (2,'b'), (3,'c')]
// Stops at shorter container

// Tuple zip
std::tuple<int, double> t1 = {1, 2.0};
std::tuple<char, float> t2 = {'a', 3.0f};
auto zipped = tuple_zip(t1, t2);  // tuple of pairs
```

---

## Common Patterns

### Bazel Build

```bazel
cc_library(
    name = "my_lib",
    srcs = ["my_lib.cpp"],
    hdrs = ["my_lib.h"],
    deps = [
        "//clim:argparse",
        "//clim:path",
        "//clim:vt",
    ],
)
```

### Include Style

All headers use `clim/` prefix:

```cpp
#include "clim/argparse.h"
#include "clim/vt/vt.h"
#include "clim/hasher/hash.h"
```

### Namespace

- Most utilities are in global namespace or `clim::`
- `vt::` namespace for vector tensor operations
- `base64::` namespace for base64 functions
