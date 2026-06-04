#ifndef VEC_H
#define VEC_H

#include "tool.h"
#include "misc.h"
#include "copy_vector.h"
#include  "factory.h"
#include"common_values.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <utility>
#include <new>


namespace ZK
{

    // Declaration of Vec
    template<class Num_T> class Vec;
    // Declaration of Mat
    template<class Num_T> class Mat;
    // Declaration of bin
    class bin;

    //-----------------------------------------------------------------------------------
    // Declaration of Vec Friends
    //-----------------------------------------------------------------------------------

    //! Addition of two vectors
    template<class Num_T>
    Vec<Num_T> operator+(const Vec<Num_T>& v1, const Vec<Num_T>& v2);
    //! Addition of a vector and a scalar
    template<class Num_T>
    Vec<Num_T> operator+(const Vec<Num_T>& v, Num_T t);
    //! Addition of a scalar and a vector
    template<class Num_T>
    Vec<Num_T> operator+(Num_T t, const Vec<Num_T>& v);

    //! Subtraction of a vector from a vector
    template<class Num_T>
    Vec<Num_T> operator-(const Vec<Num_T>& v1, const Vec<Num_T>& v2);
    //! Subtraction of a scalar from a vector
    template<class Num_T>
    Vec<Num_T> operator-(const Vec<Num_T>& v, Num_T t);
    //! Subtraction of vector from scalar. Results in a vector
    template<class Num_T>
    Vec<Num_T> operator-(Num_T t, const Vec<Num_T>& v);
    //! Negation of vector
    template<class Num_T>
    Vec<Num_T> operator-(const Vec<Num_T>& v);

    //! Inner (dot) product of two vectors v1 and v2
    template<class Num_T>
    Num_T dot(const Vec<Num_T>& v1, const Vec<Num_T>& v2);
    //! Inner (dot) product of two vectors v1 and v2
    template<class Num_T>
    Num_T operator*(const Vec<Num_T>& v1, const Vec<Num_T>& v2);
    /*!
     * \brief Outer product of two vectors v1 and v2
     *
     * When \a v1 and \a v2 are complex vectors (cvec), the third boolean
     * argument \a hermitian can be set to \a true to conjugate \a v2
     * (Matlab's v1 * v2' operation). This parameter is ignored for types
     * other then cvec.
     */
    template<class Num_T>
    Mat<Num_T> outer_product(const Vec<Num_T>& v1, const Vec<Num_T>& v2,
        bool hermitian = false);
    //! Multiplication of a vector and a scalar
    template<class Num_T>
    Vec<Num_T> operator*(const Vec<Num_T>& v, Num_T t);
    //! Multiplication of a scalar and a vector. Results in a vector
    template<class Num_T>
    Vec<Num_T> operator*(Num_T t, const Vec<Num_T>& v);

    //! Element-wise multiplication of two vectors
    template<class Num_T>
    Vec<Num_T> elem_mult(const Vec<Num_T>& a, const Vec<Num_T>& b);
    //! Element-wise multiplication of three vectors
    template<class Num_T>
    Vec<Num_T> elem_mult(const Vec<Num_T>& a, const Vec<Num_T>& b,
        const Vec<Num_T>& c);
    //! Element-wise multiplication of four vectors
    template<class Num_T>
    Vec<Num_T> elem_mult(const Vec<Num_T>& a, const Vec<Num_T>& b,
        const Vec<Num_T>& c, const Vec<Num_T>& d);

    //! Element-wise multiplication of two vectors, storing the result in vector \c out
    template<class Num_T>
    void elem_mult_out(const Vec<Num_T>& a, const Vec<Num_T>& b,
        Vec<Num_T>& out);
    //! Element-wise multiplication of three vectors, storing the result in vector \c out
    template<class Num_T>
    void elem_mult_out(const Vec<Num_T>& a, const Vec<Num_T>& b,
        const Vec<Num_T>& c, Vec<Num_T>& out);
    //! Element-wise multiplication of four vectors, storing the result in vector \c out
    template<class Num_T>
    void elem_mult_out(const Vec<Num_T>& a, const Vec<Num_T>& b,
        const Vec<Num_T>& c, const Vec<Num_T>& d,
        Vec<Num_T>& out);

    //! In-place element-wise multiplication of two vectors. Faster version of b = elem_mult(a,b).
    template<class Num_T>
    void elem_mult_inplace(const Vec<Num_T>& a, Vec<Num_T>& b);
    //! Element-wise multiplication of two vectors, followed by summation of the resultant elements. Fast version of sum(elem_mult(a,b)).
    template<class Num_T>
    Num_T elem_mult_sum(const Vec<Num_T>& a, const Vec<Num_T>& b);

    //! Division of all elements in \c v with \c t
    template<class Num_T>
    Vec<Num_T> operator/(const Vec<Num_T>& v, Num_T t);
    //! Division of \c t with all elements in \c v
    template<class Num_T>
    Vec<Num_T> operator/(Num_T t, const Vec<Num_T>& v);

    //! Elementwise division of two vectors
    template<class Num_T>
    Vec<Num_T> elem_div(const Vec<Num_T>& a, const Vec<Num_T>& b);
    //! This function is deprecated. Please use operator/(Num_T, const Vec<Num_T &) instead.
    template<class Num_T>
    Vec<Num_T> elem_div(Num_T t, const Vec<Num_T>& v);
    //! Elementwise division of two vectors, storing the result in vector \c out
    template<class Num_T>
    void elem_div_out(const Vec<Num_T>& a, const Vec<Num_T>& b, Vec<Num_T>& out);
    //! Elementwise division of two vectors, followed by summation of the resultant elements. Fast version of sum(elem_div(a,b))
    template<class Num_T>
    Num_T elem_div_sum(const Vec<Num_T>& a, const Vec<Num_T>& b);

    //! Append element \c a to the end of the vector \c v
    template<class Num_T>
    Vec<Num_T> concat(const Vec<Num_T>& v, Num_T a);
    //! Concat element \c a to the beginning of the vector \c v
    template<class Num_T>
    Vec<Num_T> concat(Num_T a, const Vec<Num_T>& v);
    //! Concat vectors \c v1 and \c v2
    template<class Num_T>
    Vec<Num_T> concat(const Vec<Num_T>& v1, const Vec<Num_T>& v2);
    //! Concat vectors \c v1, \c v2 and \c v3
    template<class Num_T>
    Vec<Num_T> concat(const Vec<Num_T>& v1, const Vec<Num_T>& v2,
        const Vec<Num_T>& v3);
    //! Concat vectors \c v1, \c v2, \c v3 and \c v4
    template<class Num_T>
    Vec<Num_T> concat(const Vec<Num_T>& v1, const Vec<Num_T>& v2,
        const Vec<Num_T>& v3, const Vec<Num_T>& v4);
    //! Concat vectors \c v1, \c v2 \c v3, \c v4 and \c v5
    template<class Num_T>
    Vec<Num_T> concat(const Vec<Num_T>& v1, const Vec<Num_T>& v2,
        const Vec<Num_T>& v3, const Vec<Num_T>& v4,
        const Vec<Num_T>& v5);

    //-----------------------------------------------------------------------------------
    // Declaration of Vec
    //-----------------------------------------------------------------------------------

    /*!
      \ingroup arr_vec_mat
      \brief Vector Class (Templated)
      \author Tony Ottosson, Tobias Ringstrom, Adam Piatyszek and Conrad Sanderson

      Vectors can be of arbitrary types, but conversions and functions are
      prepared for \c bin, \c short, \c int, \c double, and \c complex<double>
      vectors and these are predefined as: \c bvec, \c svec, \c ivec, \c vec,
      and \c cvec. \c double and \c complex<double> are \c double and
      \c complex<double> respectively.

      Examples:

      Vector Constructors:
      When constructing a vector without a length (memory) use
      \code vec temp; \endcode
      For construction of a vector of a given length use
      \code vec temp(length); \endcode
      It is also possible to assign the constructed vector the value and size
      of another vector by
      \code vec temp(invector); \endcode
      If you have explicit values you would like to assign to the vector it is
      possible to do this using strings as:
      \code
      vec a("0 0.7 5 9.3"); // that is a = [0, 0.7, 5, 9.3]
      vec a="0 0.7 5 9.3";  // the constructor is called implicitly
      ivec b="0:5";  // that is, b = [0, 1, 2, 3, 4, 5]
      vec c="3:2.5:13";  // that is, c = [3, 5.5, 8, 10.5, 13]
      \endcode
      It is also possible to change length by
      \code temp.set_size(new_length, false); \endcode
      where \c false is used to indicate that the old values in \c temp
      are not copied. If you would like to preserve the values, use \c true.

      There are a number of methods to access parts of a vector. Examples are
      \code
      a(5);     // Element number 5
      a(5,9);  // Elements 5, 6, 7, 8, and 9
      a.left(10);  // The 10 leftmost elements (the first)
      a.right(10); // The 10 rightmost elements (the last)
      a.mid(5, 7); // 7 elements starting from element 5
      \endcode

      It is also possible to modify parts of a vector, as in e.g.
      \code
      a.del(5);    // deletes element number 5
      a.ins(3.4, 9); // inserts the element 3.4 at position 9
      a.set_subvector(12, b); // replaces elements from 12 with the vector b
      \endcode

      It is, of course, also possible to perform common linear algebra
      operations, such as addition, subtraction, and scalar product (*). Observe
      though, that vectors are assumed to be column-vectors in operations with
      matrices.

      Most elementary functions such as sin(), cosh(), log(), abs(), ..., are
      also available as operations on the individual elements of the vectors. Please
      see the individual functions for more details.

      By default, the Vec elements are created using the default constructor for
      the element type. This can be changed by specifying a suitable Factory in
      the Vec constructor call; see Detailed Description for Factory.
    */
    template<class Num_T>
    class Vec
    {
    public:
        //! The type of the vector values
        typedef Num_T value_type;

        //! Default constructor. An element factory \c f can be specified.
        explicit Vec(const Factory& f = DEFAULT_FACTORY);
        //! Constructor with size parameter. An element factory \c f can be specified.
        explicit Vec(int size, const Factory& f = DEFAULT_FACTORY);
        //! Constructor with size and value parameters. An element factory \c f can be specified.
        Vec(int size, Num_T value, const Factory& f = DEFAULT_FACTORY);
        //! Copy constructor
        Vec(const Vec<Num_T>& v);
        //! Copy constructor, which takes an element factory \c f as an additional argument.
        Vec(const Vec<Num_T>& v, const Factory& f);
        //! Constructor taking a char string as input. An element factory \c f can be specified.
        Vec(const char* str, const Factory& f = DEFAULT_FACTORY);
        //! Constructor taking a string as input. An element factory \c f can be specified.
        Vec(const std::string& str, const Factory& f = DEFAULT_FACTORY);
        //! Constructor taking a C-array as input. Copies all data. An element factory \c f can be specified.
        Vec(Num_T* const c_array, int size, const Factory& f = DEFAULT_FACTORY);

        //! Destructor
        ~Vec();



        //! The size of the vector
        size_t length() const { return datasize; }
        //! The size of the vector
        size_t size() const { return datasize; }

        //! The capacity of the vector
        size_t capacity() const { return capacity_; }
        //! Set length of vector. if copy = true then keeping the old values
        void set_size(size_t size, bool copy = false);
        //! Reserve memory for at least n elements
        void reserve(size_t n);

        //! Add element to the end of the vector
        void push_back(const Num_T& val);
        //! Construct and add element to the end of the vector
        template<class... Args>
        void emplace_back(Args&&... args);

        //! Set length of vector. if copy = true then keeping the old values
        void set_length(size_t size, bool copy = false) { set_size(size, copy); }
        //! Set the vector to the all zero vector
        void zeros();
        //! Set the vector to the all zero vector
        void clear() { zeros(); }
        //! Set the vector to the all one vector
        void ones();
        //! Set the vector equal to the values in the \c str string
        void set(const char* str);
        //! Set the vector equal to the values in the \c str string
        void set(const std::string& str);

        //! C-style index operator. First element is 0.
        const Num_T& operator[](size_t i) const;
        //! Index operator. First element is 0.
        const Num_T& operator()(size_t i) const;
        //! C-style index operator. First element is 0.
        Num_T& operator[](size_t i);
        //! Index operator. First element is 0.
        Num_T& operator()(size_t i);
        //! Sub-vector with elements from \c i1 to \c i2. Index -1 indicates the last element.
        Vec<Num_T> operator()(size_t i1, size_t i2) const;
        //! Sub-vector with elements given by the list of indices \c indexlist
        Vec<Num_T> operator()(const Vec<int>& indexlist) const;
        //! Sub-vector with elements with indexes where \c binlist is \c 1
        Vec<Num_T> operator()(const Vec<bin>& binlist) const;

        //! Accessor-style method. First element is 0.
        const Num_T& get(size_t i) const;
        //! Get the elements from \c i1 to \c i2. Index -1 indicates the last element.
        Vec<Num_T> get(size_t i1, size_t i2) const;
        //! Get the elements given by the list of indices \c indexlist
        Vec<Num_T> get(const Vec<int>& indexlist) const;
        //! Get the elements with indexes where \c binlist is \c 1
        Vec<Num_T> get(const Vec<bin>& binlist) const;

        //! Modifier-style method. First element is 0.
        void set(size_t i, Num_T t);

        //! Matrix transpose. Converts to a matrix with the vector in the first row
        Mat<Num_T> transpose() const;
        //! Matrix transpose. Converts to a matrix with the vector in the first row
        Mat<Num_T> T() const { return this->transpose(); }
        //! Hermitian matrix transpose. Converts to a matrix with the conjugate of the vector in the first row
        Mat<Num_T> hermitian_transpose() const;
        //! Hermitian matrix transpose. Converts to a matrix with the conjugate of the vector in the first row
        Mat<Num_T> H() const { return this->hermitian_transpose(); }

        //! Addition of vector
        Vec<Num_T>& operator+=(const Vec<Num_T>& v);
        //! Addition of scalar
        Vec<Num_T>& operator+=(Num_T t);
        //! Addition of two vectors
        friend Vec<Num_T> operator+<>(const Vec<Num_T>& v1, const Vec<Num_T>& v2);
        //! Addition of a vector and a scalar
        friend Vec<Num_T> operator+<>(const Vec<Num_T>& v, Num_T t);
        //! Addition of a scalar and a vector
        friend Vec<Num_T> operator+<>(Num_T t, const Vec<Num_T>& v);

        //! Subtraction of vector
        Vec<Num_T>& operator-=(const Vec<Num_T>& v);
        //! Subtraction of scalar
        Vec<Num_T>& operator-=(Num_T t);
        //! Subtraction of \c v2 from \c v1
        friend Vec<Num_T> operator-<>(const Vec<Num_T>& v1, const Vec<Num_T>& v2);
        //! Subtraction of scalar from vector
        friend Vec<Num_T> operator-<>(const Vec<Num_T>& v, Num_T t);
        //! Subtraction of vector from scalar
        friend Vec<Num_T> operator-<>(Num_T t, const Vec<Num_T>& v);
        //! Negation of vector
        friend Vec<Num_T> operator-<>(const Vec<Num_T>& v);

        //! Multiply with a scalar
        Vec<Num_T>& operator*=(Num_T t);
        //! Elementwise multiplication of vector and scalar
        friend Vec<Num_T> operator*<>(const Vec<Num_T>& v, Num_T t);
        //! Elementwise multiplication of vector and scalar
        friend Vec<Num_T> operator*<>(Num_T t, const Vec<Num_T>& v);

        //! Elementwise multiplication
        friend Vec<Num_T> elem_mult<>(const Vec<Num_T>& a, const Vec<Num_T>& b);
        //! Elementwise multiplication of three vectors
        friend Vec<Num_T> elem_mult<>(const Vec<Num_T>& a, const Vec<Num_T>& b,
            const Vec<Num_T>& c);
        //! Elementwise multiplication of four vectors
        friend Vec<Num_T> elem_mult<>(const Vec<Num_T>& a, const Vec<Num_T>& b,
            const Vec<Num_T>& c, const Vec<Num_T>& d);

        //! Elementwise multiplication, storing the result in vector \c out
        friend void elem_mult_out<>(const Vec<Num_T>& a, const Vec<Num_T>& b,
            Vec<Num_T>& out);
        //! Elementwise multiplication of three vectors, storing the result in vector \c out
        friend void elem_mult_out<>(const Vec<Num_T>& a, const Vec<Num_T>& b,
            const Vec<Num_T>& c, Vec<Num_T>& out);
        //! Elementwise multiplication of four vectors, storing the result in vector \c out
        friend void elem_mult_out<>(const Vec<Num_T>& a, const Vec<Num_T>& b,
            const Vec<Num_T>& c, const Vec<Num_T>& d,
            Vec<Num_T>& out);

        //! In-place element-wise multiplication of two vectors. Fast version of b = elem_mult(a,b).
        friend void elem_mult_inplace<>(const Vec<Num_T>& a, Vec<Num_T>& b);
        //! Element-wise multiplication of two vectors, followed by summation of the resultant elements. Fast version of sum(elem_mult(a,b))
        friend Num_T elem_mult_sum<>(const Vec<Num_T>& a, const Vec<Num_T>& b);

        //! Elementwise division
        Vec<Num_T>& operator/=(Num_T t);
        //! Elementwise division
        Vec<Num_T>& operator/=(const Vec<Num_T>& v);

        //! Elementwise division
        friend Vec<Num_T> operator/<>(const Vec<Num_T>& v, Num_T t);
        //! Elementwise division
        friend Vec<Num_T> operator/<>(Num_T t, const Vec<Num_T>& v);

        //! Elementwise division
        friend Vec<Num_T> elem_div<>(const Vec<Num_T>& v1, const Vec<Num_T>& v2);
        //! This function is deprecated. Please use operator/(Num_T, const Vec<Num_T> &) instead.
        friend Vec<Num_T> elem_div<>(Num_T t, const Vec<Num_T>& v);
        //! Elementwise division
        friend void elem_div_out<>(const Vec<Num_T>& v1, const Vec<Num_T>& v2,
            Vec<Num_T>& out);
        //! Elementwise division, followed by summation of the resultant elements. Fast version of sum(elem_mult(a,b))
        friend Num_T elem_div_sum<>(const Vec<Num_T>& a, const Vec<Num_T>& b);

        //! Allocate an external buffer to the vector. The size of the buffer must match the current size of the vector.
        void setExternalBuffer(Num_T* buffer, size_t size);

        //! Get the right \c nr elements from the vector
        Vec<Num_T> right(int nr) const;
        //! Get the left \c nr elements from the vector
        Vec<Num_T> left(int nr) const;
        //! Get the middle part of vector from \c start including \c nr elements
        Vec<Num_T> mid(int start, int nr) const;
        /*!
         * \brief Split the vector into two parts at element \c pos.
         *
         * Return the first part containing elements 0 to \c pos-1, and keep the
         * second part containing the remaining elements starting from \c pos
         * (empty vector if \c pos is equal to the length of the vector).
         */
        Vec<Num_T> split(int pos);
        //! Shift in element \c t at position 0 \c n times
        void shift_right(Num_T t, int n = 1);
        //! Shift in vector \c v at position 0
        void shift_right(const Vec<Num_T>& v);
        //! Shift out the \c n left elements and at the same time shift in the element \c t at last position \c n times
        void shift_left(Num_T t, int n = 1);
        //! Shift in vector \c v at last positions
        void shift_left(const Vec<Num_T>& v);

        //! Append element \c t to the end of the vector \c v
        friend Vec<Num_T> concat<>(const Vec<Num_T>& v, Num_T t);
        //! Insert element \c t at the beginning of the vector \c v
        friend Vec<Num_T> concat<>(Num_T t, const Vec<Num_T>& v);
        //! Concatenate vectors \c v1 and \c v2
        friend Vec<Num_T> concat<>(const Vec<Num_T>& v1, const Vec<Num_T>& v2);
        //! Concatenate vectors \c v1, \c v2 and \c v3
        friend Vec<Num_T> concat<>(const Vec<Num_T>& v1, const Vec<Num_T>& v2,
            const Vec<Num_T>& v3);
        //! Concatenate vectors \c v1, \c v2, \c v3 and \c v4
        friend Vec<Num_T> concat<>(const Vec<Num_T>& v1, const Vec<Num_T>& v2,
            const Vec<Num_T>& v3, const Vec<Num_T>& v4);
        //! Concatenate vectors \c v1, \c v2, \c v3, \c v4 and \c v5
        friend Vec<Num_T> concat<>(const Vec<Num_T>& v1, const Vec<Num_T>& v2,
            const Vec<Num_T>& v3, const Vec<Num_T>& v4,
            const Vec<Num_T>& v5);

        //! Serialize to a char array
        void serializeTo(char* buffer) const;

        //! Deserialize from a char array
        void deserializeFrom(const char* bufffer);

        //! This function is deprecated. Please use set_subvector(i, v) instead.
        void set_subvector(int i1, int i2, const Vec<Num_T>& v);
        //! Set subvector to elements of vector \c v starting from element \c i
        void set_subvector(int i, const Vec<Num_T>& v);
        //! Set subvector defined by indices \c i1 and \c i2 to \c t
        void set_subvector(int i1, int i2, Num_T t);
        //! An alias function of set_subvector(i, &v)
        void replace_mid(int i, const Vec<Num_T>& v);
        //! Delete element number \c i
        void del(int i);
        //! Delete elements from \c i1 to \c i2
        void del(int i1, int i2);
        //! Insert element \c t before element with index \c i (0 <= i <= datasize)
        void ins(int i, Num_T t);
        //! Insert vector \c v before element with index \c i (0 <= i <= datasize)
        void ins(int i, const Vec<Num_T>& v);

        //! Assign all elements in vector to \c t
        Vec<Num_T>& operator=(Num_T t);
        //! Assign vector the value and length of \c v
        Vec<Num_T>& operator=(const Vec<Num_T>& v);
        //! Assign vector equal to the 1-dimensional matrix \c m
        Vec<Num_T>& operator=(const Mat<Num_T>& m);
        //! Assign vector the values in the string \c str
        Vec<Num_T>& operator=(const char* str);
        //! Assign vector the values in the string \c str
        Vec<Num_T>& operator=(const std::string& str);

        //! Elementwise equal to the scalar \c t
        Vec<bin> operator==(Num_T t) const;
        //! Elementwise not-equal to the scalar \c t
        Vec<bin> operator!=(Num_T t) const;
        //! Elementwise less than the scalar \c t
        Vec<bin> operator<(Num_T t) const;
        //! Elementwise less than and equal to the scalar \c t
        Vec<bin> operator<=(Num_T t) const;
        //! Elementwise greater than the scalar \c t
        Vec<bin> operator>(Num_T t) const;
        //! Elementwise greater than and equal to the scalar \c t
        Vec<bin> operator>=(Num_T t) const;

        //! Compare with vector \c v. Return false if sizes or values differ.
        bool operator==(const Vec<Num_T>& v) const;
        //! Compare with vector \c v. Return true if sizes or values differ.
        bool operator!=(const Vec<Num_T>& v) const;

        //! Index operator without boundary check. Not recommended for use.
        Num_T& _elem(size_t i) { return data[i]; }
        //! Index operator without boundary check. Not recommended for use.
        const Num_T& _elem(size_t i) const { return data[i]; }

        //! Get the pointer to the internal structure. Not recommended for use.
        Num_T* _data() { return data; }
        //! Get the pointer to the internal structure. Not recommended for use.
        const Num_T* _data() const { return data; }
        //! Check whether the vector uses external storage
        bool getIsExternalStorage() const { return isExternalStorage; }

    protected:
        //! Allocate storage for a vector of length \c size.
        void alloc(size_t size);
        //! Allocate storage for a vector of length \c size, and set all elements to \c value.
        void alloc(size_t size, Num_T value);
        //! Free the storage space allocated by the vector
        void free();

        //! Indicates whether the vector uses external storage
        bool isExternalStorage;

        //! The current number of elements in the vector
        size_t datasize;
        //! A pointer to the data area
        Num_T* data;
        //! The capacity of the allocated storage
        size_t capacity_;
        //! Element factory (set to DEFAULT_FACTORY to use Num_T default constructors only)
        const Factory& factory;

    private:
        // Clean up and tokenize input initialisation string
        std::vector<std::string> tokenize(const std::string& str,
            bool& abc_format) const;
        // Parse double and integer values from string tokens
        Num_T parse_token(const std::string& s) const;
        // Parse \c a, \c b and \c c values from "a:b:c" format
        void parse_abc_token(const std::string& s, Num_T& a, Num_T& b,
            Num_T& c) const;
        //! Check whether index \c i is in the allowed range
        bool in_range(size_t i) const { return ((i < datasize) && (i >= 0)); }
    };

    //-----------------------------------------------------------------------------------
    // Type definitions of vec, cvec, ivec, svec, and bvec
    //-----------------------------------------------------------------------------------

    /*!
      \relates Vec
      \brief Definition of double vector type
    */
    typedef Vec<double> vec;

    /*!
      \relates Vec
      \brief Definition of complex<double> vector type
    */
    typedef Vec<std::complex<double> > cvec;

    /*!
      \relates Vec
      \brief Definition of integer vector type
    */
    typedef Vec<int> ivec;

    /*!
      \relates Vec
      \brief Definition of short vector type
    */
    typedef Vec<short int> svec;

    /*!
      \relates Vec
      \brief Definition of binary vector type
    */
    typedef Vec<bin> bvec;

    /*!
      \relates Vec
      \brief Definition of long long vector type
    */
    typedef Vec<long long> llvec;

} //namespace ZK


#include "mat.h"

namespace ZK
{

    //-----------------------------------------------------------------------------------
    // Declaration of input and output streams for Vec
    //-----------------------------------------------------------------------------------

    /*!
      \relates Vec
      \brief Stream output of vector
    */
    template<class Num_T>
    std::ostream& operator<<(std::ostream& os, const Vec<Num_T>& v);

    /*!
      \relates Vec
      \brief Stream input of vector

      The input can be on the form "1 2 3" or "[1 2 3]", i.e. with or without
      brackets. The first form is compatible with the set method, while the
      second form is compatible with the ostream operator. The elements can be
      separated by blank space or commas. "[]" means an empty vector. "1:4"
      means "1 2 3 4". "1:3:10" means every third integer from 1 to 10, i.e.
      "1 4 7 10".
    */
    template<class Num_T>
    std::istream& operator>>(std::istream& is, Vec<Num_T>& v);

    //-----------------------------------------------------------------------------------
    // Implementation of templated Vec members and friends
    //-----------------------------------------------------------------------------------

    template<class Num_T> inline
        void Vec<Num_T>::alloc(size_t size)
    {
        if (size > 0) {
            if (size > capacity_) {
                reserve(size);
            }
            // Default construct elements
            for (size_t i = 0; i < size; ++i) {
                new (data + i) Num_T();
            }
            datasize = size;
        }
        else {
            datasize = 0;
        }
    }

    template<class Num_T> inline
        void Vec<Num_T>::alloc(size_t size, Num_T value)
    {
        if (size > 0) {
            if (size > capacity_) {
                reserve(size);
            }
            for (size_t i = 0; i < size; i++)
            {
                new (data + i) Num_T(value);
            }
            datasize = size;
        }
        else {
            datasize = 0;
        }
    }

    template<class Num_T> inline
        void Vec<Num_T>::free()
    {
        if (data) {
            for (size_t i = 0; i < datasize; ++i) {
                data[i].~Num_T();
            }
            // Only deallocate if we own the memory (managed by capacity logic usually, but here relies on destroy_elements previously)
            // To match previous create_elements/destroy_elements removal, we need to handle deallocation manually.
            // Assuming standard operator new[] was used for capacity allocation logic below.
            // But wait, the previous code called destroy_elements.
            // If isExternalStorage is true, free() is NOT called by destructor (lines 674-675).
            // So we just need to deallocate 'data'.

            // We should use operator delete (void*) to match operator new (void*)
            operator delete(static_cast<void*>(data));
        }
        data = 0;
        datasize = 0;
        capacity_ = 0;
    }


    template<class Num_T> inline
        Vec<Num_T>::Vec(const Factory& f) : datasize(0), data(0), factory(f), isExternalStorage(false), capacity_(0) {}

    template<class Num_T> inline
        Vec<Num_T>::Vec(int size, const Factory& f) : datasize(0), data(0), factory(f), isExternalStorage(false), capacity_(0)
    {
        ZKIC_Assert(size >= 0, "Negative size in Vec::Vec(int)");
        alloc(size);
    }

    template<class Num_T> inline
        Vec<Num_T>::Vec(int size, Num_T value, const Factory& f) : datasize(0), data(0), factory(f), isExternalStorage(false), capacity_(0)
    {
        ZKIC_Assert(size >= 0, "Negative size in Vec::Vec(int)");
        alloc(size, value);
    }

    template<class Num_T> inline
        Vec<Num_T>::Vec(const Vec<Num_T>& v) : datasize(0), data(0), factory(v.factory), isExternalStorage(false), capacity_(0)
    {
        if (v.getIsExternalStorage()) 
        {
			datasize = v.datasize;
			data = v.data;
			isExternalStorage = true;
			capacity_ = v.capacity_;
        }
        else 
        {
            alloc(v.datasize);
            copy_vector(datasize, v.data, data);
        }
    }

    template<class Num_T> inline
        Vec<Num_T>::Vec(const Vec<Num_T>& v, const Factory& f) : datasize(0), data(0), factory(f), isExternalStorage(false), capacity_(0)
    {
        if (v.getIsExternalStorage())
        {
            datasize = v.datasize;
            data = v.data;
            isExternalStorage = true;
            capacity_ = v.capacity_;
        }
        else
        {
            alloc(v.datasize);
            copy_vector(datasize, v.data, data);
        }
    }

    template<class Num_T> inline
        Vec<Num_T>::Vec(const char* str, const Factory& f) : datasize(0), data(0), factory(f), isExternalStorage(false), capacity_(0)
    {
        set(std::string(str));
    }

    template<class Num_T> inline
        Vec<Num_T>::Vec(const std::string& str, const Factory& f) : datasize(0), data(0), factory(f), isExternalStorage(false), capacity_(0)
    {
        set(str);
    }

    template<class Num_T> inline
        Vec<Num_T>::Vec(Num_T* const c_array, int size, const Factory& f) : datasize(size), data(c_array), factory(f), isExternalStorage(true), capacity_(size)
    {
        ZKIC_Assert(size >= 0, "Negative size in Vec::Vec(c_array, int)");
    }

    template<class Num_T> inline
        Vec<Num_T>::~Vec()
    {
        if (!isExternalStorage)
            free();
    }

    template<class Num_T>
    void Vec<Num_T>::set_size(size_t size, bool copy)
    {
        ZKIC_Assert(size >= 0, "Vec::set_size(): New size must not be negative");
        if (datasize == size)
            return;
        if (isExternalStorage)
        {
            if (size <= 2 * datasize)
            {
                datasize = size;
            }
            else
            {
                it_error("Vec::set_size(): Cannot resize vector with external storage to a larger size");
            }
        }
        else
        {
            if (!copy) {
                // Destruct all existing elements
                for (size_t i = 0; i < datasize; ++i) {
                    data[i].~Num_T();
                }
                datasize = 0;
                // If we need more capacity, reallocate.
                if (size > capacity_) {
                    reserve(size);
                }
                // Construct new elements
                for (size_t i = 0; i < size; ++i) {
                    new (data + i) Num_T();
                }
                datasize = size;
            }
            else {
                // copy == true, preserve elements
                if (size <= capacity_) {
                    if (size > datasize) {
                        for (size_t i = datasize; i < size; ++i) new (data + i) Num_T();
                    }
                    else {
                        for (size_t i = size; i < datasize; ++i) data[i].~Num_T();
                    }
                    datasize = size;
                }
                else {
                    reserve(size); // This implementation of reserve preserves elements!
                    // Now construct the new ones
                    for (size_t i = datasize; i < size; ++i) new (data + i) Num_T();
                    datasize = size;
                }
            }
        }
    }

    template<class Num_T>
    void Vec<Num_T>::reserve(size_t n)
    {
        if (n <= capacity_) return;
        if (isExternalStorage) {
            it_error("Vec::reserve(): Cannot reserve memory for external storage vector");
            return;
        }

        // Allocate raw memory
        Num_T* new_data = static_cast<Num_T*>(operator new(n * sizeof(Num_T)));

        // Move or copy existing elements
        size_t new_datasize = datasize; // preserve size
        for (size_t i = 0; i < datasize; ++i) {
            new (new_data + i) Num_T(std::move(data[i])); // Move construct
            data[i].~Num_T(); // Destroy old
        }

        // Deallocate old buffer
        if (data) {
            operator delete(static_cast<void*>(data));
        }

        data = new_data;
        capacity_ = n;
    }

    template<class Num_T>
    void Vec<Num_T>::push_back(const Num_T& val)
    {
        if (isExternalStorage) {
            it_error("Vec::push_back(): Cannot push_back to external storage vector");
            return;
        }
        if (datasize == capacity_) {
            reserve(std::max<size_t>(capacity_ * 2, 1));
        }
        new (data + datasize) Num_T(val);
        datasize++;
    }

    template<class Num_T>
    template<class... Args>
    void Vec<Num_T>::emplace_back(Args&&... args)
    {
        if (isExternalStorage) {
            it_error("Vec::emplace_back(): Cannot emplace_back to external storage vector");
            return;
        }
        if (datasize == capacity_) {
            reserve(std::max<size_t>(capacity_ * 2, 1));
        }
        new (data + datasize) Num_T(std::forward<Args>(args)...);
        datasize++;
    }

    template<class Num_T> inline
        const Num_T& Vec<Num_T>::operator[](size_t i) const
    {
        ZKIC_Assert(in_range(i), "Vec<>::operator[]: Index out of range");
        return data[i];
    }

    template<class Num_T> inline
        const Num_T& Vec<Num_T>::operator()(size_t i) const
    {
        return (*this)[i];
    }

    template<class Num_T> inline
        Num_T& Vec<Num_T>::operator[](size_t i)
    {
        ZKIC_Assert(in_range(i), "Vec<>::operator[]: Index out of range");
        return data[i];
    }

    template<class Num_T> inline
        Num_T& Vec<Num_T>::operator()(size_t i)
    {
        return (*this)[i];
    }

    template<class Num_T> inline
        Vec<Num_T> Vec<Num_T>::operator()(size_t i1, size_t i2) const
    {
        if (i1 == -1) i1 = datasize - 1;
        if (i2 == -1) i2 = datasize - 1;

        ZKIC_Assert((i1 >= 0) && (i1 <= i2) && (i2 < datasize),
            "Vec<>::operator()(i1, i2): Indexing out of range");

        Vec<Num_T> s(i2 - i1 + 1);
        copy_vector(s.datasize, data + i1, s.data);

        return s;
    }

    template<class Num_T>
    Vec<Num_T> Vec<Num_T>::operator()(const Vec<int>& indexlist) const
    {
        int size = indexlist.size();
        Vec<Num_T> temp(size);
        for (int i = 0; i < size; ++i) {
            ZKIC_Assert(in_range(indexlist(i)), "Vec<>::operator()(ivec &): "
                "Index i=" << i << " out of range");
            temp(i) = data[indexlist(i)];
        }
        return temp;
    }

    template<class Num_T>
    Vec<Num_T> Vec<Num_T>::operator()(const Vec<bin>& binlist) const
    {
        int size = binlist.size();
        ZKIC_Assert(datasize == size, "Vec<>::operator()(bvec &): "
            "Wrong size of binlist vector");
        Vec<Num_T> temp(size);
        int j = 0;
        for (int i = 0; i < size; ++i)
            if (binlist(i) == bin(1))
                temp(j++) = data[i];
        temp.set_size(j, true);
        return temp;
    }


    template<class Num_T> inline
        const Num_T& Vec<Num_T>::get(size_t i) const
    {
        return (*this)[i];
    }

    template<class Num_T> inline
        Vec<Num_T> Vec<Num_T>::get(size_t i1, size_t i2) const
    {
        return (*this)(i1, i2);
    }

    template<class Num_T> inline
        Vec<Num_T> Vec<Num_T>::get(const Vec<int>& indexlist) const
    {
        return (*this)(indexlist);
    }

    template<class Num_T> inline
        Vec<Num_T> Vec<Num_T>::get(const Vec<bin>& binlist) const
    {
        return (*this)(binlist);
    }

    template<class Num_T> inline
        void Vec<Num_T>::zeros()
    {
        for (size_t i = 0; i < datasize; i++)
            data[i] = Num_T(0);
    }

    template<class Num_T> inline
        void Vec<Num_T>::ones()
    {
        for (size_t i = 0; i < datasize; i++)
            data[i] = Num_T(1);
    }

    template<class Num_T> inline
        void Vec<Num_T>::set(size_t i, Num_T t)
    {
        ZKIC_Assert(in_range(i), "Vec<>::set(i, t): Index out of range");
        data[i] = t;
    }

    template<class Num_T> inline
        void Vec<Num_T>::set(const std::string& str)
    {
        it_error("Vec::set(): Only `double', `complex<double>', `int', "
            "`short int' and `bin' types supported");
    }

    template<class Num_T> inline
        void Vec<Num_T>::set(const char* str)
    {
        set(std::string(str));
    }

    //! \cond
    template<>
    ZKIC_DLL_EXPORT void Vec<double>::set(const std::string& str);
    template<>
    ZKIC_DLL_EXPORT void Vec<std::complex<double> >::set(const std::string& str);
    template<>
    ZKIC_DLL_EXPORT void Vec<int>::set(const std::string& str);
    template<>
    ZKIC_DLL_EXPORT void Vec<short int>::set(const std::string& str);
    template<>
    ZKIC_DLL_EXPORT void Vec<bin>::set(const std::string& str);
    template<>
    ZKIC_DLL_EXPORT void Vec<long long>::set(const std::string& str);
    //! \endcond

    template<class Num_T>
    Mat<Num_T> Vec<Num_T>::transpose() const
    {
        Mat<Num_T> temp(1, datasize);
        copy_vector(datasize, data, temp._data());
        return temp;
    }

    template<class Num_T>
    Mat<Num_T> Vec<Num_T>::hermitian_transpose() const
    {
        Mat<Num_T> temp(1, datasize);
        copy_vector(datasize, data, temp._data());
        return temp;
    }

    //! \cond
    template<>
    ZKIC_DLL_EXPORT Mat<std::complex<double> > Vec<std::complex<double> >::hermitian_transpose() const;
    //! \endcond


    template<class Num_T>
    Vec<Num_T>& Vec<Num_T>::operator+=(const Vec<Num_T>& v)
    {
        if (datasize == 0) { // if not assigned a size.
            if (this != &v) { // check for self addition
                alloc(v.datasize);
                copy_vector(datasize, v.data, data);
            }
        }
        else {
            ZKIC_Assert(datasize == v.datasize, "Vec::operator+=: Wrong sizes");
            for (int i = 0; i < datasize; i++)
                data[i] += v.data[i];
        }
        return *this;
    }

    template<class Num_T> inline
        Vec<Num_T>& Vec<Num_T>::operator+=(Num_T t)
    {
        for (int i = 0;i < datasize;i++)
            data[i] += t;
        return *this;
    }

    template<class Num_T>
    Vec<Num_T> operator+(const Vec<Num_T>& v1, const Vec<Num_T>& v2)
    {
        int i;
        Vec<Num_T> r(v1.datasize);

        ZKIC_Assert(v1.datasize == v2.datasize, "Vec::operator+: wrong sizes");
        for (i = 0; i < v1.datasize; i++)
            r.data[i] = v1.data[i] + v2.data[i];

        return r;
    }

    template<class Num_T>
    Vec<Num_T> operator+(const Vec<Num_T>& v, Num_T t)
    {
        int i;
        Vec<Num_T> r(v.datasize);

        for (i = 0; i < v.datasize; i++)
            r.data[i] = v.data[i] + t;

        return r;
    }

    template<class Num_T>
    Vec<Num_T> operator+(Num_T t, const Vec<Num_T>& v)
    {
        int i;
        Vec<Num_T> r(v.datasize);

        for (i = 0; i < v.datasize; i++)
            r.data[i] = t + v.data[i];

        return r;
    }

    template<class Num_T>
    Vec<Num_T>& Vec<Num_T>::operator-=(const Vec<Num_T>& v)
    {
        if (datasize == 0) { // if not assigned a size.
            if (this != &v) { // check for self decrementation
                alloc(v.datasize);
                for (int i = 0; i < v.datasize; i++)
                    data[i] = -v.data[i];
            }
        }
        else {
            ZKIC_Assert(datasize == v.datasize, "Vec::operator-=: Wrong sizes");
            for (int i = 0; i < datasize; i++)
                data[i] -= v.data[i];
        }
        return *this;
    }

    template<class Num_T> inline
        Vec<Num_T>& Vec<Num_T>::operator-=(Num_T t)
    {
        for (int i = 0;i < datasize;i++)
            data[i] -= t;
        return *this;
    }

    template<class Num_T>
    Vec<Num_T> operator-(const Vec<Num_T>& v1, const Vec<Num_T>& v2)
    {
        int i;
        Vec<Num_T> r(v1.datasize);

        ZKIC_Assert(v1.datasize == v2.datasize, "Vec::operator-: wrong sizes");
        for (i = 0; i < v1.datasize; i++)
            r.data[i] = v1.data[i] - v2.data[i];

        return r;
    }

    template<class Num_T>
    Vec<Num_T> operator-(const Vec<Num_T>& v, Num_T t)
    {
        int i;
        Vec<Num_T> r(v.datasize);

        for (i = 0; i < v.datasize; i++)
            r.data[i] = v.data[i] - t;

        return r;
    }

    template<class Num_T>
    Vec<Num_T> operator-(Num_T t, const Vec<Num_T>& v)
    {
        int i;
        Vec<Num_T> r(v.datasize);

        for (i = 0; i < v.datasize; i++)
            r.data[i] = t - v.data[i];

        return r;
    }

    template<class Num_T>
    Vec<Num_T> operator-(const Vec<Num_T>& v)
    {
        int i;
        Vec<Num_T> r(v.datasize);

        for (i = 0; i < v.datasize; i++)
            r.data[i] = -v.data[i];

        return r;
    }

    template<class Num_T> inline
        Vec<Num_T>& Vec<Num_T>::operator*=(Num_T t)
    {
        scal_vector(datasize, t, data);
        return *this;
    }

    template<class Num_T> inline
        Num_T operator*(const Vec<Num_T>& v1, const Vec<Num_T>& v2)
    {
        return dot(v1, v2);
    }

    template<class Num_T>
    Num_T dot(const Vec<Num_T>& v1, const Vec<Num_T>& v2)
    {
        ZKIC_Assert(v1.length() == v2.length(), "Vec::dot(): Wrong sizes");
        Num_T r = Num_T(0);
        for (int i = 0; i < v1.length(); ++i)
            r += v1._data()[i] * v2._data()[i];
        return r;
    }

    //! \cond
    template<>
    ZKIC_DLL_EXPORT double dot(const vec& v1, const vec& v2);
    //! \endcond


    template<class Num_T>
    Mat<Num_T> outer_product(const Vec<Num_T>& v1, const Vec<Num_T>& v2, bool)
    {
        ZKIC_Assert((v1.length() > 0) && (v2.length() > 0),
            "Vec::outer_product:: Input vector of zero size");

        Mat<Num_T> r(v1.length(), v2.length());
        for (int i = 0; i < v1.length(); ++i) {
            for (int j = 0; j < v2.length(); ++j) {
                r(i, j) = v1._data()[i] * v2._data()[j];
            }
        }
        return r;
    }

    //! \cond
    template<>
    ZKIC_DLL_EXPORT mat outer_product(const vec& v1, const vec& v2, bool);

    template<>
    ZKIC_DLL_EXPORT cmat outer_product(const cvec& v1, const cvec& v2, bool hermitian);
    //! \endcond

    template<class Num_T>
    Vec<Num_T> operator*(const Vec<Num_T>& v, Num_T t)
    {
        int i;
        Vec<Num_T> r(v.datasize);
        for (i = 0; i < v.datasize; i++)
            r.data[i] = v.data[i] * t;

        return r;
    }

    template<class Num_T> inline
        Vec<Num_T> operator*(Num_T t, const Vec<Num_T>& v)
    {
        return operator*(v, t);
    }

    template<class Num_T> inline
        Vec<Num_T> elem_mult(const Vec<Num_T>& a, const Vec<Num_T>& b)
    {
        Vec<Num_T> out;
        elem_mult_out(a, b, out);
        return out;
    }

    template<class Num_T> inline
        Vec<Num_T> elem_mult(const Vec<Num_T>& a, const Vec<Num_T>& b,
            const Vec<Num_T>& c)
    {
        Vec<Num_T> out;
        elem_mult_out(a, b, c, out);
        return out;
    }

    template<class Num_T> inline
        Vec<Num_T> elem_mult(const Vec<Num_T>& a, const Vec<Num_T>& b,
            const Vec<Num_T>& c, const Vec<Num_T>& d)
    {
        Vec<Num_T> out;
        elem_mult_out(a, b, c, d, out);
        return out;
    }

    template<class Num_T>
    void elem_mult_out(const Vec<Num_T>& a, const Vec<Num_T>& b, Vec<Num_T>& out)
    {
        ZKIC_Assert(a.datasize == b.datasize,
            "Vec<>::elem_mult_out(): Wrong sizes");
        out.set_size(a.datasize);
        for (int i = 0; i < a.datasize; i++)
            out.data[i] = a.data[i] * b.data[i];
    }

    template<class Num_T>
    void elem_mult_out(const Vec<Num_T>& a, const Vec<Num_T>& b,
        const Vec<Num_T>& c, Vec<Num_T>& out)
    {
        ZKIC_Assert((a.datasize == b.datasize) && (a.datasize == c.datasize),
            "Vec<>::elem_mult_out(): Wrong sizes");
        out.set_size(a.datasize);
        for (int i = 0; i < a.datasize; i++)
            out.data[i] = a.data[i] * b.data[i] * c.data[i];
    }

    template<class Num_T>
    void elem_mult_out(const Vec<Num_T>& a, const Vec<Num_T>& b,
        const Vec<Num_T>& c, const Vec<Num_T>& d, Vec<Num_T>& out)
    {
        ZKIC_Assert((a.datasize == b.datasize) && (a.datasize == c.datasize)
            && (a.datasize == d.datasize),
            "Vec<>::elem_mult_out(): Wrong sizes");
        out.set_size(a.datasize);
        for (int i = 0; i < a.datasize; i++)
            out.data[i] = a.data[i] * b.data[i] * c.data[i] * d.data[i];
    }

    template<class Num_T>
#ifndef _MSC_VER
    inline
#endif
        void elem_mult_inplace(const Vec<Num_T>& a, Vec<Num_T>& b)
    {
        ZKIC_Assert(a.datasize == b.datasize,
            "Vec<>::elem_mult_inplace(): Wrong sizes");
        for (int i = 0; i < a.datasize; i++)
            b.data[i] *= a.data[i];
    }

    template<class Num_T> inline
        Num_T elem_mult_sum(const Vec<Num_T>& a, const Vec<Num_T>& b)
    {
        ZKIC_Assert(a.datasize == b.datasize,
            "Vec<>::elem_mult_sum(): Wrong sizes");
        Num_T acc = 0;
        for (int i = 0; i < a.datasize; i++)
            acc += a.data[i] * b.data[i];
        return acc;
    }

    template<class Num_T>
    Vec<Num_T> operator/(const Vec<Num_T>& v, Num_T t)
    {
        int i;
        Vec<Num_T> r(v.datasize);

        for (i = 0; i < v.datasize; i++)
            r.data[i] = v.data[i] / t;

        return r;
    }

    template<class Num_T>
    Vec<Num_T> operator/(Num_T t, const Vec<Num_T>& v)
    {
        int i;
        Vec<Num_T> r(v.datasize);

        for (i = 0; i < v.datasize; i++)
            r.data[i] = t / v.data[i];

        return r;
    }

    template<class Num_T>
    Vec<Num_T> elem_div(Num_T t, const Vec<Num_T>& v)
    {
        it_warning("Vec<>::elem_div(Num_T, const Vec<Num_T> &): This function is "
            "deprecated and might be removed from future IT++ releases. "
            "Please use Vec<>::operator/(Num_T, const Vec<Num_T> &) "
            "instead.");
        return operator/(t, v);
    }

    template<class Num_T> inline
        Vec<Num_T>& Vec<Num_T>::operator/=(Num_T t)
    {
        for (int i = 0; i < datasize; ++i) {
            data[i] /= t;
        }
        return *this;
    }

    template<class Num_T> inline
        Vec<Num_T>& Vec<Num_T>::operator/=(const Vec<Num_T>& v)
    {
        ZKIC_Assert(datasize == v.datasize, "Vec::operator/=(): wrong sizes");
        for (int i = 0; i < datasize; ++i) {
            data[i] /= v.data[i];
        }
        return *this;
    }

    template<class Num_T> inline
        Vec<Num_T> elem_div(const Vec<Num_T>& a, const Vec<Num_T>& b)
    {
        Vec<Num_T> out;
        elem_div_out(a, b, out);
        return out;
    }

    template<class Num_T>
    void elem_div_out(const Vec<Num_T>& a, const Vec<Num_T>& b, Vec<Num_T>& out)
    {
        ZKIC_Assert(a.datasize == b.datasize, "Vecelem_div_out: wrong sizes");

        out.set_size(a.size());

        for (int i = 0; i < a.datasize; i++)
            out.data[i] = a.data[i] / b.data[i];
    }

    template<class Num_T> inline
        Num_T elem_div_sum(const Vec<Num_T>& a, const Vec<Num_T>& b)
    {
        ZKIC_Assert(a.datasize == b.datasize, "Vec::elem_div_sum: wrong sizes");

        Num_T acc = 0;

        for (int i = 0; i < a.datasize; i++)
            acc += a.data[i] / b.data[i];

        return acc;
    }

    template<class Num_T>
    inline void Vec<Num_T>::setExternalBuffer(Num_T* buffer, size_t size)
    {
        isExternalStorage = true;
        data = buffer;
        datasize = size;
    }

    template<class Num_T>
    Vec<Num_T> Vec<Num_T>::right(int nr) const
    {
        ZKIC_Assert(nr <= datasize, "Vec::right(): index out of range");
        Vec<Num_T> temp(nr);
        if (nr > 0) {
            copy_vector(nr, &data[datasize - nr], temp.data);
        }
        return temp;
    }

    template<class Num_T>
    Vec<Num_T> Vec<Num_T>::left(int nr) const
    {
        ZKIC_Assert(nr <= datasize, "Vec::left(): index out of range");
        Vec<Num_T> temp(nr);
        if (nr > 0) {
            copy_vector(nr, data, temp.data);
        }
        return temp;
    }

    template<class Num_T>
    Vec<Num_T> Vec<Num_T>::mid(int start, int nr) const
    {
        ZKIC_Assert((start >= 0) && ((start + nr) <= datasize),
            "Vec::mid(): indexing out of range");
        Vec<Num_T> temp(nr);
        if (nr > 0) {
            copy_vector(nr, &data[start], temp.data);
        }
        return temp;
    }

    template<class Num_T>
    Vec<Num_T> Vec<Num_T>::split(int pos)
    {
        ZKIC_Assert((pos >= 0) && (pos <= datasize),
            "Vec<>::split(): Index out of range");
        Vec<Num_T> temp1(pos);
        if (pos > 0) {
            copy_vector(pos, data, temp1.data);
            if (pos < datasize) {
                Vec<Num_T> temp2(datasize - pos);
                copy_vector(datasize - pos, &data[pos], temp2.data);
                (*this) = temp2;
            }
            else {
                set_size(0);
            }
        }
        return temp1;
    }

    template<class Num_T>
    void Vec<Num_T>::shift_right(Num_T t, int n)
    {
        int i = datasize;

        ZKIC_Assert(n >= 0, "Vec::shift_right: index out of range");
        while (--i >= n)
            data[i] = data[i - n];
        while (i >= 0)
            data[i--] = t;
    }

    template<class Num_T>
    void Vec<Num_T>::shift_right(const Vec<Num_T>& v)
    {
        for (int i = datasize - 1; i >= v.datasize; i--)
            data[i] = data[i - v.datasize];
        for (int i = 0; i < v.datasize; i++)
            data[i] = v[i];
    }

    template<class Num_T>
    void Vec<Num_T>::shift_left(Num_T t, int n)
    {
        int i;

        ZKIC_Assert(n >= 0, "Vec::shift_left: index out of range");
        for (i = 0; i < datasize - n; i++)
            data[i] = data[i + n];
        while (i < datasize)
            data[i++] = t;
    }

    template<class Num_T>
    void Vec<Num_T>::shift_left(const Vec<Num_T>& v)
    {
        for (int i = 0; i < datasize - v.datasize; i++)
            data[i] = data[i + v.datasize];
        for (int i = datasize - v.datasize; i < datasize; i++)
            data[i] = v[i - datasize + v.datasize];
    }

    template<class Num_T>
    Vec<Num_T> concat(const Vec<Num_T>& v, Num_T t)
    {
        int size = v.size();
        Vec<Num_T> temp(size + 1);
        copy_vector(size, v.data, temp.data);
        temp(size) = t;
        return temp;
    }

    template<class Num_T>
    Vec<Num_T> concat(Num_T t, const Vec<Num_T>& v)
    {
        int size = v.size();
        Vec<Num_T> temp(size + 1);
        temp(0) = t;
        copy_vector(size, v.data, &temp.data[1]);
        return temp;
    }

    template<class Num_T>
    Vec<Num_T> concat(const Vec<Num_T>& v1, const Vec<Num_T>& v2)
    {
        int size1 = v1.size();
        int size2 = v2.size();
        Vec<Num_T> temp(size1 + size2);
        copy_vector(size1, v1.data, temp.data);
        copy_vector(size2, v2.data, &temp.data[size1]);
        return temp;
    }

    template<class Num_T>
    Vec<Num_T> concat(const Vec<Num_T>& v1, const Vec<Num_T>& v2,
        const Vec<Num_T>& v3)
    {
        int size1 = v1.size();
        int size2 = v2.size();
        int size3 = v3.size();
        Vec<Num_T> temp(size1 + size2 + size3);
        copy_vector(size1, v1.data, temp.data);
        copy_vector(size2, v2.data, &temp.data[size1]);
        copy_vector(size3, v3.data, &temp.data[size1 + size2]);
        return temp;
    }

    template<class Num_T>
    Vec<Num_T> concat(const Vec<Num_T>& v1, const Vec<Num_T>& v2,
        const Vec<Num_T>& v3, const Vec<Num_T>& v4)
    {
        int size1 = v1.size();
        int size2 = v2.size();
        int size3 = v3.size();
        int size4 = v4.size();
        Vec<Num_T> temp(size1 + size2 + size3 + size4);
        copy_vector(size1, v1.data, temp.data);
        copy_vector(size2, v2.data, &temp.data[size1]);
        copy_vector(size3, v3.data, &temp.data[size1 + size2]);
        copy_vector(size4, v4.data, &temp.data[size1 + size2 + size3]);
        return temp;
    }

    template<class Num_T>
    Vec<Num_T> concat(const Vec<Num_T>& v1, const Vec<Num_T>& v2,
        const Vec<Num_T>& v3, const Vec<Num_T>& v4,
        const Vec<Num_T>& v5)
    {
        int size1 = v1.size();
        int size2 = v2.size();
        int size3 = v3.size();
        int size4 = v4.size();
        int size5 = v5.size();
        Vec<Num_T> temp(size1 + size2 + size3 + size4 + size5);
        copy_vector(size1, v1.data, temp.data);
        copy_vector(size2, v2.data, &temp.data[size1]);
        copy_vector(size3, v3.data, &temp.data[size1 + size2]);
        copy_vector(size4, v4.data, &temp.data[size1 + size2 + size3]);
        copy_vector(size5, v5.data, &temp.data[size1 + size2 + size3 + size4]);
        return temp;
    }

    template<class Num_T>
    inline void Vec<Num_T>::serializeTo(char* buffer) const
    {
        if (datasize == 0) return;

        if constexpr (std::is_same<Num_T, bin>::value)
        {
            for (size_t i = 0;i < datasize;++i)
            {
                buffer[i] = static_cast<char>((data[i] == bin(1)) ? 1 : 0);
            }
        }
        else if constexpr (std::is_same<Num_T, std::complex<double>>::value)
        {
            for (size_t i = 0;i < datasize;++i)
            {
                double realPart = std::real(data[i]);
                double imagPart = std::imag(data[i]);

                std::memcpy((buffer + i * 2 * sizeof(double)), &realPart, sizeof(double));
                std::memcpy((buffer + (i * 2 + 1) * sizeof(double)), &imagPart, sizeof(double));
            }
        }
        else
        {
            std::memcpy(buffer, data, datasize * sizeof(Num_T));
        }
    }

    template<class Num_T>
    inline void Vec<Num_T>::deserializeFrom(const char* buffer)
    {
        if (datasize == 0)return;

        if constexpr (std::is_same<Num_T, bin>::value)
        {
            for (size_t i = 0;i < datasize;++i)
            {
                data[i] = (buffer[i] == 0) ? bin(0) : bin(1);
            }
        }
        else if constexpr (std::is_same<Num_T, std::complex<double>>::value)
        {
            for (size_t i = 0;i < datasize;++i)
            {
                double realPart = 0.0, imagPart = 0.0;
                std::memcpy(&realPart, buffer + i * 2 * sizeof(double), sizeof(double));
                std::memcpy(&imagPart, buffer + (i * 2 + 1) * sizeof(double), sizeof(double));
                data[i] = std::complex<double>(realPart, imagPart);
            }
        }
        else
        {
            memcpy(data, buffer, datasize * sizeof(Num_T));
        }
    }

    template<class Num_T>
    void Vec<Num_T>::set_subvector(int i1, int, const Vec<Num_T>& v)
    {
        it_warning("Vec<>::set_subvector(int, int, const Vec<> &): This function "
            "is deprecated and might be removed from future IT++ releases. "
            "Please use Vec<>::set_subvector(int, const Vec<> &) instead.");
        set_subvector(i1, v);
    }

    template<class Num_T> inline
        void Vec<Num_T>::set_subvector(int i, const Vec<Num_T>& v)
    {
        ZKIC_Assert((i >= 0) && (i + v.datasize <= datasize),
            "Vec<>::set_subvector(int, const Vec<> &): "
            "Index out of range or too long input vector");
        copy_vector(v.datasize, v.data, data + i);
    }

    template<class Num_T> inline
        void Vec<Num_T>::set_subvector(int i1, int i2, Num_T t)
    {
        if (i1 == -1) i1 = datasize - 1;
        if (i2 == -1) i2 = datasize - 1;
        ZKIC_Assert((i1 >= 0) && (i1 <= i2) && (i2 < datasize),
            "Vec<>::set_subvector(int, int, Num_T): Indexing out "
            "of range");
        for (int i = i1; i <= i2; i++)
            data[i] = t;
    }

    template<class Num_T> inline
        void Vec<Num_T>::replace_mid(int i, const Vec<Num_T>& v)
    {
        set_subvector(i, v);
    }

    template<class Num_T>
    void Vec<Num_T>::del(int index)
    {
        ZKIC_Assert(in_range(index), "Vec<>::del(int): Index out of range");
        Vec<Num_T> temp(*this);
        set_size(datasize - 1, false);
        copy_vector(index, temp.data, data);
        copy_vector(datasize - index, &temp.data[index + 1], &data[index]);
    }

    template<class Num_T>
    void Vec<Num_T>::del(int i1, int i2)
    {
        if (i1 == -1) i1 = datasize - 1;
        if (i2 == -1) i2 = datasize - 1;
        ZKIC_Assert((i1 >= 0) && (i1 <= i2) && (i2 < datasize),
            "Vec<>::del(int, int): Indexing out of range");
        Vec<Num_T> temp(*this);
        int new_size = datasize - (i2 - i1 + 1);
        set_size(new_size, false);
        copy_vector(i1, temp.data, data);
        copy_vector(datasize - i1, &temp.data[i2 + 1], &data[i1]);
    }

    template<class Num_T>
    void Vec<Num_T>::ins(int index, const Num_T t)
    {
        ZKIC_Assert((index >= 0) && (index <= datasize),
            "Vec<>::ins(): Index out of range");
        Vec<Num_T> Temp(*this);

        set_size(datasize + 1, false);
        copy_vector(index, Temp.data, data);
        data[index] = t;
        copy_vector(Temp.datasize - index, Temp.data + index, data + index + 1);
    }

    template<class Num_T>
    void Vec<Num_T>::ins(int index, const Vec<Num_T>& v)
    {
        ZKIC_Assert((index >= 0) && (index <= datasize),
            "Vec<>::ins(): Index out of range");
        Vec<Num_T> Temp(*this);

        set_size(datasize + v.length(), false);
        copy_vector(index, Temp.data, data);
        copy_vector(v.size(), v.data, &data[index]);
        copy_vector(Temp.datasize - index, Temp.data + index, data + index + v.size());
    }

    template<class Num_T> inline
        Vec<Num_T>& Vec<Num_T>::operator=(Num_T t)
    {
        for (int i = 0;i < datasize;i++)
            data[i] = t;
        return *this;
    }

    template<class Num_T> inline
        Vec<Num_T>& Vec<Num_T>::operator=(const Vec<Num_T>& v)
    {
        if (v.getIsExternalStorage())
        {
            data = const_cast<Num_T*>(v._data());
            datasize = v.size();
            isExternalStorage = true;
        }
        else
        {
            if (this != &v) {
                set_size(v.datasize, false);
                copy_vector(datasize, v.data, data);
            }
        }
        return *this;
    }

    template<class Num_T>
    Vec<Num_T>& Vec<Num_T>::operator=(const Mat<Num_T>& m)
    {
        if (m.getIsExternalStorage())
        {
            if (m.cols() == 1) {
                datasize = m.rows();
            }
            else if (m.rows() == 1) {
                datasize = m.cols();
            }
            else
            {
                it_error("Vec<>::operator=(Mat<Num_T> &): Wrong size of input matrix");
            }
            data = const_cast<Num_T*>(m._data());
            isExternalStorage = true;
        }
        else
        {
            if (m.cols() == 1) {
                set_size(m.rows(), false);
                copy_vector(m.rows(), m._data(), data);
            }
            else if (m.rows() == 1) {
                set_size(m.cols(), false);
                copy_vector(m.cols(), m._data(), m.rows(), data, 1);
            }
            else
                it_error("Vec<>::operator=(Mat<Num_T> &): Wrong size of input matrix");
        }
        return *this;
    }

    template<class Num_T> inline
        Vec<Num_T>& Vec<Num_T>::operator=(const char* str)
    {
        set(std::string(str));
        return *this;
    }

    template<class Num_T> inline
        Vec<Num_T>& Vec<Num_T>::operator=(const std::string& str)
    {
        set(str);
        return *this;
    }

    template<class Num_T>
    bvec Vec<Num_T>::operator==(Num_T t) const
    {
        ZKIC_Assert(datasize > 0, "Vec<>::operator==(): Wrong size");
        bvec temp(datasize);
        for (int i = 0; i < datasize; i++)
            temp(i) = (data[i] == t);
        return temp;
    }

    template<class Num_T>
    bvec Vec<Num_T>::operator!=(Num_T t) const
    {
        ZKIC_Assert(datasize > 0, "Vec<>::operator!=(): Wrong size");
        bvec temp(datasize);
        for (int i = 0; i < datasize; i++)
            temp(i) = (data[i] != t);
        return temp;
    }

    //! \cond
    template<>
    bvec Vec<std::complex<double> >::operator<(std::complex<double>) const;
    //! \endcond

    template<class Num_T>
    bvec Vec<Num_T>::operator<(Num_T t) const
    {
        ZKIC_Assert(datasize > 0, "Vec<>::operator<(): Wrong size");
        bvec temp(datasize);
        for (int i = 0; i < datasize; i++)
            temp(i) = (data[i] < t);
        return temp;
    }

    template<class Num_T>
    bvec Vec<Num_T>::operator<=(Num_T t) const
    {
        ZKIC_Assert(datasize > 0, "Vec<>::operator<=(): Wrong size");
        bvec temp(datasize);
        for (int i = 0; i < datasize; i++)
            temp(i) = (data[i] <= t);
        return temp;
    }

    //! \cond
    template<>
    ZKIC_DLL_EXPORT bvec Vec<std::complex<double> >::operator<=(std::complex<double>) const;
    //! \endcond

    template<class Num_T>
    bvec Vec<Num_T>::operator>(Num_T t) const
    {
        ZKIC_Assert(datasize > 0, "Vec<>::operator>(): Wrong size");
        bvec temp(datasize);
        for (int i = 0; i < datasize; i++)
            temp(i) = (data[i] > t);
        return temp;
    }

    //! \cond
    template<>
    ZKIC_DLL_EXPORT bvec Vec<std::complex<double> >::operator>(std::complex<double>) const;
    //! \endcond

    template<class Num_T>
    bvec Vec<Num_T>::operator>=(Num_T t) const
    {
        ZKIC_Assert(datasize > 0, "Vec<>::operator>=(): Wrong size");
        bvec temp(datasize);
        for (int i = 0; i < datasize; i++)
            temp(i) = (data[i] >= t);
        return temp;
    }

    //! \cond
    template<>
    ZKIC_DLL_EXPORT bvec Vec<std::complex<double> >::operator>=(std::complex<double>) const;
    //! \endcond

    template<class Num_T>
    bool Vec<Num_T>::operator==(const Vec<Num_T>& invector) const
    {
        // OBS ! if wrong size, return false
        if (datasize != invector.datasize) return false;
        for (int i = 0;i < datasize;i++) {
            if (data[i] != invector.data[i]) return false;
        }
        return true;
    }

    template<class Num_T>
    bool Vec<Num_T>::operator!=(const Vec<Num_T>& invector) const
    {
        if (datasize != invector.datasize) return true;
        for (int i = 0;i < datasize;i++) {
            if (data[i] != invector.data[i]) return true;
        }
        return false;
    }

    //! Output stream operator of a vector \c v
    template<class Num_T>
    std::ostream& operator<<(std::ostream& os, const Vec<Num_T>& v)
    {
        int i, sz = v.length();

        os << "[";
        for (i = 0; i < sz; i++) {
            os << v(i);
            if (i < sz - 1)
                os << " ";
        }
        os << "]";

        return os;
    }

    //! Input stream operator to read a vector
    template<class Num_T>
    std::istream& operator>>(std::istream& is, Vec<Num_T>& v)
    {
        std::ostringstream buffer;
        bool started = false;
        bool finished = false;
        bool brackets = false;
        char c = 0;

        while (!finished) {
            if (is.eof()) {
                finished = true;
            }
            else {
                is.get(c);

                if (is.eof() || (c == '\n')) {
                    if (brackets) {
                        // Right bracket missing
                        is.setstate(std::ios_base::failbit);
                        finished = true;
                    }
                    else if (!((c == '\n') && !started)) {
                        finished = true;
                    }
                }
                else if ((c == ' ') || (c == '\t')) {
                    if (started) {
                        buffer << ' ';
                    }
                }
                else if (c == '[') {
                    if (started) {
                        // Unexpected left bracket
                        is.setstate(std::ios_base::failbit);
                        finished = true;
                    }
                    else {
                        started = true;
                        brackets = true;
                    }
                }
                else if (c == ']') {
                    if (!started || !brackets) {
                        // Unexpected right bracket
                        is.setstate(std::ios_base::failbit);
                        finished = true;
                    }
                    else {
                        finished = true;
                    }
                    while (!is.eof() && (((c = static_cast<char>(is.peek())) == ' ')
                        || (c == '\t'))) {
                        is.get();
                    }
                    if (!is.eof() && (c == '\n')) {
                        is.get();
                    }
                }
                else {
                    started = true;
                    buffer << c;
                }
            }
        }

        if (!started) {
            v.set_size(0, false);
        }
        else {
            v.set(buffer.str());
        }

        return is;
    }

    //! \cond

    // ----------------------------------------------------------------------
    // Private functions
    // ----------------------------------------------------------------------

    template<class Num_T>
    void Vec<Num_T>::parse_abc_token(const std::string& s, Num_T& a, Num_T& b,
        Num_T& c) const
    {
        std::string::size_type beg = 0;
        std::string::size_type end = s.find(':', 0);
        a = parse_token(s.substr(beg, end - beg));
        beg = end + 1;
        end = s.find(':', beg);
        if (end != std::string::npos) {
            b = parse_token(s.substr(beg, end - beg));
            c = parse_token(s.substr(end + 1, s.size() - end));
        }
        else {
            b = Num_T(1);
            c = parse_token(s.substr(beg, end - beg - 1));
        }
    }

    template<class Num_T>
    Num_T Vec<Num_T>::parse_token(const std::string& s) const
    {
        it_error("Vec::parse_token(): Only `double' and `int' types are supported");
        return 0;
    }

    //! \cond
    template<>
    ZKIC_DLL_EXPORT double Vec<double>::parse_token(const std::string& s) const;
    template<>
    ZKIC_DLL_EXPORT int Vec<int>::parse_token(const std::string& s) const;
    template<>
    ZKIC_DLL_EXPORT long long Vec<long long>::parse_token(const std::string& s) const;
    //! \endcond

    template<class Num_T>
    std::vector<std::string> Vec<Num_T>::tokenize(const std::string& str_in,
        bool& abc_format) const
    {
        std::vector<std::string> vs;  // vector for storing parsed tokens
        std::string s;                // currently processed token string
        bool start = true;
        bool space = false;
        bool colon = false;
        bool comma = false;
        bool lparen = false;
        abc_format = false;
        for (std::string::size_type i = 0; i < str_in.size(); ++i) {
            char c = str_in[i];
            switch (c) {
            case '[':
                start = true;
                break;
            case ']':
                start = true;
                break;
            case ' ': case '\t':
                space = true;             // set flag for whitespaces
                break;
            case ',':
                if (lparen)
                    comma = true;           // set flag for comma in "(re,im)" format
                else
                    space = true;           // otherwise treat comma as separator
                break;
            case ')':
                s.push_back('i');         // replace right paren in "(re,im)" with 'i'
                break;
            case ':':
                colon = true;             // set flag for "a:b[:c]" format string
                space = false;            // reset flag for whitespaces
                abc_format = true;        // set external flag for "a:b[:c]" format
                s.push_back(c);
                break;
            case '(':
                lparen = true;            // set flag for complex "(re,im)" format
                break;
            default:
                if (colon) {              // reset colon and space flags
                    colon = false;          // to get rid of whitespaces around ":"
                    space = false;
                }
                else if (lparen && comma) { // support for "(re,im)" format
                    lparen = false;
                    comma = false;
                    space = false;
                    if ((c != '-') && (c != '+')) // if needed
                        s.push_back('+');           // insert '+' between "re" and "im"
                }
                else if (space) {         // new token detected
                    space = false;
                    if (!start) {           // if not at the beginning of the string
                        vs.push_back(s);      // store already parsed token
                        s.clear();            // and start parsing the next token
                    }
                }
                s.push_back(c);          // append next character to the current token
                start = false;           // reset the "beginning of the string" flag
                break;
            }
        }
        if (!s.empty())               // if the final token is not an empty string
            vs.push_back(s);            // store it in the output vector
        return vs;
    }

    // ----------------------------------------------------------------------
    // Instantiations
    // ---------------------------------------------------------------------- 
    ZKIC_DLL_EXPORT_TEMPLATE template class ZKIC_DLL_EXPORT Vec<double>;
    ZKIC_DLL_EXPORT_TEMPLATE template class ZKIC_DLL_EXPORT Vec<int>;
    ZKIC_DLL_EXPORT_TEMPLATE template class ZKIC_DLL_EXPORT Vec<short int>;
    ZKIC_DLL_EXPORT_TEMPLATE template class ZKIC_DLL_EXPORT Vec<std::complex<double> >;
    ZKIC_DLL_EXPORT_TEMPLATE template class ZKIC_DLL_EXPORT Vec<bin>;
    ZKIC_DLL_EXPORT_TEMPLATE template class ZKIC_DLL_EXPORT Vec<long long>;

    // addition operator

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec operator+(const vec& v1, const vec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec operator+(const cvec& v1, const cvec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec operator+(const ivec& v1, const ivec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec operator+(const svec& v1, const svec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec operator+(const bvec& v1, const bvec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec operator+(const llvec& v1, const llvec& v2);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec operator+(const vec& v1, double t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec operator+(const cvec& v1, std::complex<double> t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec operator+(const ivec& v1, int t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec operator+(const svec& v1, short t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec operator+(const bvec& v1, bin t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec operator+(const llvec& v1, long long t);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec operator+(double t, const vec& v1);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec operator+(std::complex<double> t, const cvec& v1);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec operator+(int t, const ivec& v1);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec operator+(short t, const svec& v1);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec operator+(bin t, const bvec& v1);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec operator+(long long t, const llvec& v1);

    // subtraction operator

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec operator-(const vec& v1, const vec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec operator-(const cvec& v1, const cvec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec operator-(const ivec& v1, const ivec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec operator-(const svec& v1, const svec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec operator-(const bvec& v1, const bvec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec operator-(const llvec& v1, const llvec& v2);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec operator-(const vec& v, double t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec operator-(const cvec& v, std::complex<double> t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec operator-(const ivec& v, int t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec operator-(const svec& v, short t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec operator-(const bvec& v, bin t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec operator-(const llvec& v, long long t);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec operator-(double t, const vec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec operator-(std::complex<double> t, const cvec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec operator-(int t, const ivec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec operator-(short t, const svec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec operator-(bin t, const bvec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec operator-(long long t, const llvec& v);

    // unary minus

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec operator-(const vec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec operator-(const cvec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec operator-(const ivec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec operator-(const svec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec operator-(const bvec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec operator-(const llvec& v);

    // multiplication operator
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT std::complex<double> dot(const cvec& v1, const cvec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT int dot(const ivec& v1, const ivec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT short dot(const svec& v1, const svec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bin dot(const bvec& v1, const bvec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT long long dot(const llvec& v1, const llvec& v2);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT double operator*(const vec& v1, const vec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT std::complex<double> operator*(const cvec& v1, const cvec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT int operator*(const ivec& v1, const ivec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT short operator*(const svec& v1, const svec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bin operator*(const bvec& v1, const bvec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT long long operator*(const llvec& v1, const llvec& v2);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT imat outer_product(const ivec& v1, const ivec& v2,
        bool hermitian);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT smat outer_product(const svec& v1, const svec& v2,
        bool hermitian);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bmat outer_product(const bvec& v1, const bvec& v2,
        bool hermitian);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec operator*(const vec& v, double t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec operator*(const cvec& v, std::complex<double> t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec operator*(const ivec& v, int t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec operator*(const svec& v, short t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec operator*(const bvec& v, bin t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec operator*(const llvec& v, long long t);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec operator*(double t, const vec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec operator*(std::complex<double> t, const cvec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec operator*(int t, const ivec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec operator*(short t, const svec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec operator*(bin t, const bvec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec operator*(long long t, const llvec& v);

    // elementwise multiplication

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec elem_mult(const vec& a, const vec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec elem_mult(const cvec& a, const cvec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec elem_mult(const ivec& a, const ivec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec elem_mult(const svec& a, const svec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec elem_mult(const bvec& a, const bvec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec elem_mult(const llvec& a, const llvec& b);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const vec& a, const vec& b, vec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const cvec& a, const cvec& b, cvec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const ivec& a, const ivec& b, ivec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const svec& a, const svec& b, svec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const bvec& a, const bvec& b, bvec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const llvec& a, const llvec& b, llvec& out);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec elem_mult(const vec& a, const vec& b, const vec& c);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec elem_mult(const cvec& a, const cvec& b, const cvec& c);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec elem_mult(const ivec& a, const ivec& b, const ivec& c);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec elem_mult(const svec& a, const svec& b, const svec& c);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec elem_mult(const bvec& a, const bvec& b, const bvec& c);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec elem_mult(const llvec& a, const llvec& b, const llvec& c);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const vec& a, const vec& b,
        const vec& c, vec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const cvec& a, const cvec& b,
        const cvec& c, cvec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const ivec& a, const ivec& b,
        const ivec& c, ivec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const svec& a, const svec& b,
        const svec& c, svec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const bvec& a, const bvec& b,
        const bvec& c, bvec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const llvec& a, const llvec& b,
        const llvec& c, llvec& out);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec elem_mult(const vec& a, const vec& b,
        const vec& c, const vec& d);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec elem_mult(const cvec& a, const cvec& b,
        const cvec& c, const cvec& d);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec elem_mult(const ivec& a, const ivec& b,
        const ivec& c, const ivec& d);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec elem_mult(const svec& a, const svec& b,
        const svec& c, const svec& d);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec elem_mult(const bvec& a, const bvec& b,
        const bvec& c, const bvec& d);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec elem_mult(const llvec& a, const llvec& b,
        const llvec& c, const llvec& d);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const vec& a, const vec& b, const vec& c,
        const vec& d, vec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const cvec& a, const cvec& b,
        const cvec& c, const cvec& d, cvec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const ivec& a, const ivec& b,
        const ivec& c, const ivec& d, ivec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const svec& a, const svec& b,
        const svec& c, const svec& d, svec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const bvec& a, const bvec& b,
        const bvec& c, const bvec& d, bvec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_out(const llvec& a, const llvec& b,
        const llvec& c, const llvec& d, llvec& out);

    // in-place elementwise multiplication

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_inplace(const vec& a, vec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_inplace(const cvec& a, cvec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_inplace(const ivec& a, ivec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_inplace(const svec& a, svec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_inplace(const bvec& a, bvec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_mult_inplace(const llvec& a, llvec& b);

    // elementwise multiplication followed by summation

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT double elem_mult_sum(const vec& a, const vec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT std::complex<double> elem_mult_sum(const cvec& a,
        const cvec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT int elem_mult_sum(const ivec& a, const ivec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT short elem_mult_sum(const svec& a, const svec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bin elem_mult_sum(const bvec& a, const bvec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT long long elem_mult_sum(const llvec& a, const llvec& b);

    // division operator

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec operator/(const vec& v, double t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec operator/(const cvec& v, std::complex<double> t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec operator/(const ivec& v, int t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec operator/(const svec& v, short t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec operator/(const bvec& v, bin t);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec operator/(const llvec& v, long long t);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec operator/(double t, const vec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec operator/(std::complex<double> t, const cvec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec operator/(int t, const ivec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec operator/(short t, const svec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec operator/(bin t, const bvec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec operator/(long long t, const llvec& v);

    // elementwise division operator

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec elem_div(const vec& a, const vec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec elem_div(const cvec& a, const cvec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec elem_div(const ivec& a, const ivec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec elem_div(const svec& a, const svec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec elem_div(const bvec& a, const bvec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec elem_div(const llvec& a, const llvec& b);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec elem_div(double t, const vec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec elem_div(std::complex<double> t, const cvec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec elem_div(int t, const ivec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec elem_div(short t, const svec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec elem_div(bin t, const bvec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec elem_div(long long t, const llvec& v);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_div_out(const vec& a, const vec& b, vec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_div_out(const cvec& a, const cvec& b, cvec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_div_out(const ivec& a, const ivec& b, ivec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_div_out(const svec& a, const svec& b, svec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_div_out(const bvec& a, const bvec& b, bvec& out);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void elem_div_out(const llvec& a, const llvec& b, llvec& out);

    // elementwise division followed by summation

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT double elem_div_sum(const vec& a, const vec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT std::complex<double> elem_div_sum(const cvec& a,
        const cvec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT int elem_div_sum(const ivec& a, const ivec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT short elem_div_sum(const svec& a, const svec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bin elem_div_sum(const bvec& a, const bvec& b);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT long long elem_div_sum(const llvec& a, const llvec& b);

    // concat operator

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec concat(const vec& v, double a);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec concat(const cvec& v, std::complex<double> a);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec concat(const ivec& v, int a);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec concat(const svec& v, short a);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec concat(const bvec& v, bin a);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec concat(const llvec& v, long long a);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec concat(double a, const vec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec concat(std::complex<double> a, const cvec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec concat(int a, const ivec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec concat(short a, const svec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec concat(bin a, const bvec& v);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec concat(long long a, const llvec& v);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec concat(const vec& v1, const vec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec concat(const cvec& v1, const cvec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec concat(const ivec& v1, const ivec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec concat(const svec& v1, const svec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec concat(const bvec& v1, const bvec& v2);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec concat(const llvec& v1, const llvec& v2);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec concat(const vec& v1, const vec& v2, const vec& v3);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec concat(const cvec& v1, const cvec& v2, const cvec& v3);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec concat(const ivec& v1, const ivec& v2, const ivec& v3);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec concat(const svec& v1, const svec& v2, const svec& v3);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec concat(const bvec& v1, const bvec& v2, const bvec& v3);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec concat(const llvec& v1, const llvec& v2, const llvec& v3);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec concat(const vec& v1, const vec& v2,
        const vec& v3, const vec& v4);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec concat(const cvec& v1, const cvec& v2,
        const cvec& v3, const cvec& v4);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec concat(const ivec& v1, const ivec& v2,
        const ivec& v3, const ivec& v4);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec concat(const svec& v1, const svec& v2,
        const svec& v3, const svec& v4);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec concat(const bvec& v1, const bvec& v2,
        const bvec& v3, const bvec& v4);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec concat(const llvec& v1, const llvec& v2,
        const llvec& v3, const llvec& v4);

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec concat(const vec& v1, const vec& v2, const vec& v3,
        const vec& v4, const vec& v5);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec concat(const cvec& v1, const cvec& v2, const cvec& v3,
        const cvec& v4, const cvec& v5);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec concat(const ivec& v1, const ivec& v2, const ivec& v3,
        const ivec& v4, const ivec& v5);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec concat(const svec& v1, const svec& v2, const svec& v3,
        const svec& v4, const svec& v5);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec concat(const bvec& v1, const bvec& v2, const bvec& v3,
        const bvec& v4, const bvec& v5);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT llvec concat(const llvec& v1, const llvec& v2, const llvec& v3,
        const llvec& v4, const llvec& v5);

    // I/O streams

    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT std::ostream& operator<<(std::ostream& os, const vec& vect);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT std::ostream& operator<<(std::ostream& os, const cvec& vect);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT std::ostream& operator<<(std::ostream& os, const svec& vect);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT std::ostream& operator<<(std::ostream& os, const ivec& vect);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT std::ostream& operator<<(std::ostream& os, const bvec& vect);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT std::ostream& operator<<(std::ostream& os, const llvec& vect);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT std::istream& operator>>(std::istream& is, vec& vect);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT std::istream& operator>>(std::istream& is, cvec& vect);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT std::istream& operator>>(std::istream& is, svec& vect);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT std::istream& operator>>(std::istream& is, ivec& vect);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT std::istream& operator>>(std::istream& is, bvec& vect);
    ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT std::istream& operator>>(std::istream& is, llvec& vect);

    //! \endcond


} // namespace ZK

#endif // #ifndef VEC_H
