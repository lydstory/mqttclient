#if !defined(_MSC_VER)&&__cplusplus<=201103L
namespace std{
// 支持普通指针
template<typename T,typename ...Args>inline
typename enable_if<!is_array<T>::value, unique_ptr<T> >::type
make_unique(Args&&...args){
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}
// T必须是动态数组类型，且不能是定长数组
template<typename T>inline
typename enable_if<is_array<T>::value&& extent<T>::value==0,unique_ptr<T>>::type
make_unique(size_t size){
    using U=typename remove_extent<T>::type;
    return unique_ptr<T>(new U[size]());
}
template<typename T,typename ...Args>
typename enable_if<extent<T>::value != 0,   void>::type
make_unique(Args&&...)=delete;
}
#endif /* !defined(_MSC_VER)&&__cplusplus<=201103L */
