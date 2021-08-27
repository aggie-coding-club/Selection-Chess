#ifndef MODULAR_INT_HPP
#define MODULAR_INT_HPP

#include <iostream>
#include "utils.h"

// Arithmetic operations on objects of this class are done with a modulo, and always return type ModularInt.
// m_value will always be a value in {0, 1, ..., modulus-1}.
// To get back to normal arithmetic, get the m_value.
// Templated by a pointer to the modulus. This means each class created from template has a modulus shared by all its objects, 
// and that modulus can be modified.
// The behavior for a ModularInt object is not defined when modulus changes during its existence.
template <unsigned int* modulus>
class ModularInt {
    public:
        // Value stored by a specific object
        int m_value; // Even though this only contains nonnegative values, we keep it signed so it doesn't overflow on negative numbers.

        ModularInt(int _x);
        ModularInt() : m_value(0) { }

        ModularInt& operator+=(int _x) {
            m_value += _x;
            m_value = positiveModulo(m_value, *modulus);
            return *this;
        }
        ModularInt& operator-=(int _x) {
            m_value -= _x;
            m_value = positiveModulo(m_value, *modulus);
            return *this;
        }
        ModularInt& operator+=(const ModularInt _x) {
            m_value += _x.m_value;
            m_value = positiveModulo(m_value, *modulus);
            return *this;
        }
        ModularInt& operator-=(const ModularInt _x) {
            m_value -= _x.m_value;
            m_value = positiveModulo(m_value, *modulus);
            return *this;
        }
        ModularInt& operator++() {
            m_value = positiveModulo(m_value + 1, *modulus);
            return *this;
        }
        ModularInt& operator--() {
            m_value = positiveModulo(m_value - 1, *modulus);
            return *this;
        }
        // TODO: maybe add postfix operators?

        bool operator==(const ModularInt& _other) const {
            return m_value == _other.m_value;
        }
        bool operator!=(const ModularInt& _other) const {
            return m_value != _other.m_value;
        }

        // Compares which int is more to the left or down from another, based on where the wrap-around happens (namely, at _relZero)
        // returns if this < _other
        bool lessThan(ModularInt _other, ModularInt _relZero) const;
        // Compares which int is more to the left or down from another, based on where the wrap-around happens (namely, at _relZero)
        // returns if this <= _other
        bool lessThanOrEqual(ModularInt _other, ModularInt _relZero) const;
        // TODO: remove lessThan and lessThanOrEqual, now deprecated by the following
        // Return true if this is between _lower and _upper, inclusive;
        // i.e. starting at _lower and traveling positive, you will hit this before you hit _upper.
        bool isBetween(ModularInt _lower, ModularInt _upper) const;

        // Heuristic Less Than. Maybe better name would be 'shortSideLessThan' or 'noWrapLessThan'.
        // when _lhs and _rhs are close together, we can ignore the wrap-around and perform comparisons, i.e. less than.
        // Note that the behavior of this function is undefined when the difference between _lhs and _rhs is close to ±1/2 the modulus
        bool heurLessThan(ModularInt _rhs) const;

        // Gets distance from *this to _target as a signed number, ignoring wrap around (similar to heurLessThan).
        // E.g. if _target+5 == *this, and modulus ≫ 5, then -5 is returned.
        // Note that the behavior of this function is undefined when the difference between _start and _end is close to ±1/2 the modulus
        int getDistTo(const ModularInt& _target) const;

        friend std::ostream& operator<<(std::ostream& _os, const ModularInt<modulus>& _mi) {
            _os << _mi.m_value; // << " (mod " << *modulus << ")";
            return _os;
        }
};


//-------------- Implementations below ---------------//
template <unsigned int* modulus>
inline ModularInt<modulus>::ModularInt(int _x) {
    m_value = positiveModulo(_x, *modulus);
}

// Addition with ints
template <unsigned int* modulus>
inline ModularInt<modulus> operator+(ModularInt<modulus> _mi, int _x) {
    _mi += _x;
    return _mi;
}
template <unsigned int* modulus>
inline ModularInt<modulus> operator+(int _x, ModularInt<modulus> _mi) {
    return _mi + _x;
}
// Addition with other
template <unsigned int* modulus>
inline ModularInt<modulus> operator+(ModularInt<modulus> _mi1, ModularInt<modulus> _mi2) {
    _mi1 += _mi2;
    return _mi1;
}
// Nice shorthand for adding negative ints
template <unsigned int* modulus>
inline ModularInt<modulus> operator-(ModularInt<modulus> _mi, int _x) {
    _mi -= _x;
    return _mi;
}
// Negation
template <unsigned int* modulus>
inline ModularInt<modulus> operator-(ModularInt<modulus> _mi) {
    return ModularInt<modulus>(-_mi.m_value);
}
template <unsigned int* modulus>
inline ModularInt<modulus> operator-(int _x, ModularInt<modulus> _mi) {
    return _x + (-_mi);
}
// Subtraction with other
template <unsigned int* modulus>
inline ModularInt<modulus> operator-(ModularInt<modulus> _mi1, ModularInt<modulus> _mi2) {
    _mi1 -= _mi2;
    return _mi1;
}

// Deprecated // TODO: remove
template <unsigned int* modulus>
inline bool ModularInt<modulus>::lessThan(ModularInt<modulus> _other, ModularInt<modulus> _relZero) const {
    return (m_value - _relZero).m_value < (_other.m_value - _relZero).m_value;
}
// Deprecated // TODO: remove
template <unsigned int* modulus>
inline bool ModularInt<modulus>::lessThanOrEqual(ModularInt<modulus> _other, ModularInt<modulus> _relZero) const {
    return (m_value - _relZero).m_value <= (_other.m_value - _relZero).m_value;
}


template <unsigned int* modulus>
inline bool ModularInt<modulus>::isBetween(ModularInt<modulus> _lower, ModularInt<modulus> _upper) const {
    return (m_value - _lower).m_value <= (_upper.m_value - _lower).m_value; // TODO: explain why I am converting to m_values then subtracting a modular? Is this a mistake?
}

template <unsigned int* modulus>
inline bool ModularInt<modulus>::heurLessThan(ModularInt _rhs) const {
    return (_rhs - *this).m_value < (*modulus) / 2;
}

template <unsigned int* modulus>
inline int ModularInt<modulus>::getDistTo(const ModularInt& _target) const {
    if (_target.heurLessThan(*this)) {
        return -((*this - _target).m_value); // Avoids wrap-around by negating after converted to int.
    } else {
        return (_target - *this).m_value;
    }
}


#endif