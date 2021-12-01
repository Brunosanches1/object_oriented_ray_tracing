#include <ostream>
#include <iostream>
#include <vector>

#ifndef VECTOR_STREAM_HPP
#define VECTOR_STREAM_HPP

template <typename T>
class VectorStream {
    private:
        std::vector<T> vec;
    public:
        VectorStream() : vec() {};
        VectorStream(unsigned int size) : vec(size) {}
        VectorStream(const VectorStream& original) : vec(original.vec) {};

        template<typename L>
        friend VectorStream<L>& operator<<(VectorStream<L>& vs, L elem);

        template <typename L>
        friend VectorStream<L>& operator<<(VectorStream<L>& vs, int elem);

        void clear() {
            vec.clear();
        }

        T* data() {
            return vec.data();
        }

        void resize(unsigned int new_size) {
            vec.resize(new_size);
        }

};

template <typename T>
extern VectorStream<T>& operator<<(VectorStream<T>& vs, T elem) {
    vs.vec.push_back(elem);
    return vs;
}

template <typename T>
extern VectorStream<T>& operator<<(VectorStream<T>& vs, int elem) {
    std::cout<<elem;
    return vs;
}

#endif