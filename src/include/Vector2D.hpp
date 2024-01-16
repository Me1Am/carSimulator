/** Template class for a generic vector2D 
 * The type is specified by "T" which is supplied by the programmer
 */
template <class T> class Vector2D {
    public:
		/** Default Constructor
		 * @note Sets the vector to 0 
		*/
		Vector2D() {
			x = 0;
			y = 0;
		}
		/** X, Y Constructor
		 * @param x The initial X value
		 * @param y The initial Y value
		 * @brief Sets the vector to the given numbers
		 */
		Vector2D(const T x, const T y) {
        	this->x = x;
			this->y = y;
		}
		/** Vector Constructor
		 * @param vector The vector to be copied
		 * @brief Initializes X and Y to the supplied vector
		 */
		template <typename V> Vector2D(const Vector2D<V>& vector) {
			x = vector.x;
			y = vector.y;
		}
		T x;
		T y;
        
		/// Prefix Increment
		Vector2D<T> operator ++() {
			x++;
			y++;
			return *this;
		}
		/// Postfix Increment
		Vector2D<T> operator ++(int) {
			Vector2D<T> temp = *this;
			++*this;
			return temp;
		}
		/// Prefix Decrement
		Vector2D<T> operator --() {
			x--;
			y--;
			return *this;
		}
		/// Postfix Decrement
		Vector2D<T> operator --(int) {
			Vector2D<T> temp = *this;
			--*this;
			return temp;
		}
		/// Addition Assignment
		Vector2D<T> operator +=(const Vector2D<T> postfix) {
			this->x += postfix.x;
			this->y += postfix.y;
			return *this;
		}
		/// Subtraction Assignment
		Vector2D<T> operator -=(const Vector2D<T> postfix) {
			this->x -= postfix.x;
			this->y -= postfix.y;
			return *this;
		}
		/// Multiplication Assignment
		Vector2D<T> operator *=(const Vector2D<T> postfix) {
			this->x *= postfix.x;
			this->y *= postfix.y;
			return *this;
		}
		/// Division Assignment
		Vector2D<T> operator /=(const Vector2D<T> postfix) {
			this->x /= postfix.x;
			this->y /= postfix.y;
			return *this;
		}
		/// Modulus Assignment
		Vector2D<T> operator %=(const Vector2D<T> postfix) {
			this->x %= postfix.x;
			this->y %= postfix.y;
			return *this;
		}
		/// Negate
		Vector2D<T> operator -(const Vector2D<T> postfix) {
			return Vector2D<T>(-postfix.x, -postfix.y);
		}
};

/// Addition
template <typename T> Vector2D<T> operator +(const Vector2D<T> prefix, const Vector2D<T> postfix) {
	return Vector2D<T>(prefix.x + postfix.x, prefix.y + postfix.y);
}
/// Subtraction
template <typename T> Vector2D<T> operator -(const Vector2D<T> prefix, const Vector2D<T> postfix) {
	return Vector2D<T>(prefix.x - postfix.x, prefix.y - postfix.y);
}
/// Multiplication
template <typename T> Vector2D<T> operator *(const Vector2D<T> prefix, const Vector2D<T> postfix) {
	return Vector2D<T>(prefix.x * postfix.x, prefix.y * postfix.y);
}
/// Multiplication with non-Vector prefix
template <typename T> Vector2D<T> operator *(const T prefix, const Vector2D<T> postfix) {
	return Vector2D<T>(prefix * postfix.x, prefix * postfix.y);
}
/// Multiplication with non-Vector postfix
template <typename T> Vector2D<T> operator *(const Vector2D<T> prefix, const T postfix) {
	return Vector2D<T>(prefix.x * postfix, prefix.y * postfix);
}
/// Division
template <typename T> Vector2D<T> operator /(const Vector2D<T> prefix, const Vector2D<T> postfix) {
	return Vector2D<T>(prefix.x / postfix.x, prefix.y / postfix.y);
}
/// Division with non-Vector prefix
template <typename T> Vector2D<T> operator /(const T prefix, const Vector2D<T> postfix) {
	return Vector2D<T>(prefix / postfix.x, prefix / postfix.y);
}
/// Division with non-Vector postfix
template <typename T> Vector2D<T> operator /(const Vector2D<T> prefix, const T postfix) {
	return Vector2D<T>(prefix.x / postfix, prefix.y / postfix);
}
/// Modulus
template <typename T> Vector2D<T> operator %(const Vector2D<T> prefix, const Vector2D<T> postfix) {
	return Vector2D<T>(prefix.x % postfix.x, prefix.y % postfix.y);
}
/// Equals
template <typename T> Vector2D<T> operator ==(const Vector2D<T> prefix, const Vector2D<T> postfix) {
	return (prefix.x == postfix.x && prefix.y == postfix.y);
}
/// Not-Equals
template <typename T> Vector2D<T> operator !=(const Vector2D<T> prefix, const Vector2D<T> postfix) {
	return (prefix.x != postfix.x || prefix.y != postfix.y);
}

// Predefines the most common types
// Gives unique names for each
typedef Vector2D<int> Vector2Di;
typedef Vector2D<unsigned int> Vector2Du;
typedef Vector2D<float> Vector2Df;
typedef Vector2D<double> Vector2Dd;