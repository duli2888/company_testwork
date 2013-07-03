#include <iostream>
#include <iterator>
#include <memory>

using namespace std;

template <class T>
class Vec
{
	public:
		typedef T* iterator;
		typedef const T* const_iterator;								// Vec<T>::const_iterator 相当于 const T*;
		typedef size_t size_type;										// Vec<int>::size_type 相当于 size_t;
		typedef T value_type;											// Vec<T>::value_type 相当于 T;
		typedef T& reference;											// Vec<int>::reference 相当于 to T&;
		typedef const T& const_reference;								// Vec<int>::const_reference 相当于 const T&;

		Vec() { create(); }												// 默认的构造函数
		explicit Vec(size_type n, const T& t = t()) { create(n, t); }	// 单参数或者两个参数构造函数, explicit 只对构造函数起作用，用来抑制隐式转换
		Vec(const Vec& v) { create(v.begin(), v.end()); }				// 拷贝构造函数
		Vec & operator = (const Vec&);									// 赋值构造函数
		~Vec() { uncreate(); }											// 析构函数

		size_type size() { return avail - data; }						// 定义类的大小，ptrdiff_t自动转化成size_t

		void push_back(const T& t)
		{
			if (avail == limit) {
				grow();
			}
			unchecked_append(t);
		}
		// 重载【】
		T& operator[] (size_type i) { return data[i]; }
		const T& operator[] (size_type i) const { return data[i]; }
		// 定义begin和end，都有两个版本
		iterator begin() { return data; }
		const_iterator begin() const { return data; }
		iterator end() { return avail; }
		const_iterator end() const { return avail; }
	protected:
	private:
		iterator data;						// Vec中得初始值
		iterator avail;						// Vec中得结束值
		iterator limit;						// Vec中空间分配的结束值
		std::allocator<T> alloc;			// 注意此处std
		// 创造函数，负责内存管理
		void create();
		void create(size_type, const T&);
		void create(const_iterator, const_iterator);
		// 销毁元素，返回内存
		void uncreate();
		// 支持push_back函数
		void grow();
		void unchecked_append(const T&);
};
 
//#pragma comment(lib,"ws2_32.lib")
 
// 拷贝构造函数
template <class T>
Vec<T>& Vec<T>::operator = (const Vec& v)
{
    if (&v != this) {						// 检查是否为自我赋值，很重要，必须有
        uncreate();							// 清空左值的元素
        create(v.begin(), v.end());			// 拷贝元素到左值
    }
    return *this;
}
 
// push_back函数中内存增长策略函数
template <class T>
void Vec<T>::grow()
{
    size_type new_size = max(2 * (limit - data), ptrdiff_t(1));		// 防止刚开始内存空间为0的情况
    iterator new_data = alloc.allocate(new_size);					// 返回首地址
    // 把前两个参数指定的元素复制给第三个参数表示的目标序列，返回末尾元素的下一个迭代器
    iterator new_avail = uninitialized_copy(data, avail, new_data); 
    uncreate(); //释放原先的空间
 
    data = new_data;
    avail = new_avail;
    limit = data + new_size;
}
 
// 向申请的内存中添加元素
template <class T>
void Vec<T>::unchecked_append(const T& val)
{
    // 在未初始化的空间构建一个对象，参数1插入对象的位置指针，参数2需要添加的对象
    alloc.construct(avail++, val); 
}
 
// 申请内存的函数create
template <class T>
void Vec<T>::create()
{
    data = avail = limit = 0;
}

template <class T>
void Vec<T>::create(size_type n, const T& val)
{
    data = alloc.allocate(n);				// 申请内存空间，但是不初始化
    limit = avail = data + n;
    uninitialized_fill(data, limit, val);	// 进行初始化
}

template <class T>
void Vec<T>::create(const_iterator i, const_iterator j)
{
    data = alloc.allocate(j - i);
    limit = avail = uninitialized_copy(i, j, data);
}
 
// 回收内存
template <class T>
void Vec<T>::uncreate()
{
    if (data) {									// 如果data是0，我们不需要做什么工作
        iterator it = avail;
        while (it != data)
            alloc.destroy(--it);				// 销毁没个元素，为了与delete行为一致，采用从后向前遍历
        alloc.deallocate(data, limit - data);	// 内存释放，函数需要一个非零指针
												// 因此，检测data是否为零
    }
    data = limit = avail = 0;
}
 
int main()
{
    Vec<int> a;
    Vec<int> b;
    a.push_back(12);
    b = a;
    return 0;
}
