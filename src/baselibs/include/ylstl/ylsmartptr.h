#ifndef _YLSTL_SMART_PTR_HEADER_
#define _YLSTL_SMART_PTR_HEADER_
#include <stdio.h>

struct YLRefCount
{
    // 被引用次数
    size_t nUseCount;
    // 弱引用次数
    size_t nWeakCount;

    YLRefCount(size_t use, size_t weak)
        : nUseCount(use), nWeakCount(weak)
    {}
};


template <typename T> class YLWeakPtr;
namespace yl
{
	template <class T> class enable_shared_from_this;
}

template <typename T>
class YLSmartPtr
{
    template <typename S>
    friend class YLWeakPtr;
    template <typename S>
    friend class YLSmartPtr;

public:
    explicit YLSmartPtr(T * p = NULL)
        : ptr(p), pRefCount(p ? new YLRefCount(1, 0) : NULL)
    {
        if (p)
        {
            SetSharedPtr(this, p);
        }
    }

    YLSmartPtr(const YLSmartPtr & src)
        : ptr(src.ptr), pRefCount(src.pRefCount)
    {
        IncrUse();
    }

    template <typename S>
    YLSmartPtr(const YLSmartPtr<S> & src)
        : ptr(dynamic_cast<T *>(src.ptr)), pRefCount(src.pRefCount)
    {
        IncrUse();
    }

    ~YLSmartPtr()
    {
        DecrUse();
    }

    YLSmartPtr & operator=(const YLSmartPtr & rhs)
    {
        // self-assigning is also right
        // 先增加引用再减少引用，避免自赋值时被释放
        if (rhs.pRefCount)
        {
            ++rhs.pRefCount->nUseCount;
        }
        DecrUse();
        ptr = rhs.ptr;
        pRefCount = rhs.pRefCount;
        return *this;
    }

    template <typename S>
    YLSmartPtr & operator=(const YLSmartPtr<S> & rhs)
    {
        // self-assigning is also right
        // 先增加引用再减少引用，避免自赋值时被释放
        if (rhs.pRefCount)
        {
            ++rhs.pRefCount->nUseCount;
        }
        DecrUse();
        ptr = dynamic_cast<T *>(rhs.ptr);
        pRefCount = rhs.pRefCount;
        return *this;
    }

    T * operator->()
    {
#ifdef IF_NO_EXCEPTION
        return ptr;
#else
        if (ptr)
        {
            return ptr;
        }
        throw ("Access to the NULL pointer");
#endif // IF_NO_EXCEPTION
    }

    const T * operator->() const
    {
#ifdef IF_NO_EXCEPTION
        return ptr;
#else
        if (ptr)
        {
            return ptr;
        }
        throw ("Access to the NULL pointer");
#endif // IF_NO_EXCEPTION
    }

    T & operator*()
    {
        if (ptr)
        {
            return *ptr;
        }
#ifdef IF_NO_EXCEPTION
        return NULL;
#else
        throw ("Access to the NULL pointer");
#endif // IF_NO_EXCEPTION
    }

    const T & operator*() const
    {
        if (ptr)
        {
            return *ptr;
        }
#ifdef IF_NO_EXCEPTION
        return NULL;
#else
        throw ("Access to the NULL pointer");
#endif // IF_NO_EXCEPTION
    }

    // 重新关联指针
    void Reset(T * p = 0)
    {
        if (ptr != p)
        {
            DecrUse();
            ptr = p;
            pRefCount = p ? new YLRefCount(1, 0) : NULL;
        }
    }

    // 判断指针是否为空
    bool IsEmpty() const
    {
        return !ptr;
    }

    // 获取指针
    T * Get() const
    {
        return ptr;
    }

private:
    inline void SetSharedPtr(YLSmartPtr<T> * sp, yl::enable_shared_from_this<T> * raw)
    {
        raw->_internal_accept_owner(sp);
    }

    inline void SetSharedPtr(...)
    {}

    // 增加引用计数
    void IncrUse()
    {
        if (pRefCount)
        {
            ++pRefCount->nUseCount;
        }
    }

    // 减少引用计数
    void DecrUse()
    {
        if (pRefCount)
        {
            if (pRefCount->nUseCount == 1)
            {
                if (ptr)
                {
                    delete ptr;
                    ptr = NULL;
                }
                if (pRefCount->nWeakCount == 0)
                {
                    delete pRefCount;
                    pRefCount = NULL;
                }
                else
                {
                    --pRefCount->nUseCount;
                }
            }
            else
            {
                --pRefCount->nUseCount;
            }
        }
    }

private:
    //关联的指针
    T * ptr;
    // 被引用次数
    YLRefCount * pRefCount;
};


template <typename T>
class YLWeakPtr
{
    template <typename S>
    friend class YLWeakPtr;

public:
    YLWeakPtr()
        : ptr(NULL), pRefCount(NULL)
    {}

    YLWeakPtr(const YLWeakPtr & src)
        : ptr(src.ptr), pRefCount(src.pRefCount)
    {
        IncrUse();
    }

    template <typename S>
    YLWeakPtr(const YLWeakPtr<S> & src)
        : ptr(dynamic_cast<T *>(src.ptr)), pRefCount(src.pRefCount)
    {
        IncrUse();
    }

    YLWeakPtr(const YLSmartPtr<T> & src)
        : ptr(src.ptr), pRefCount(src.pRefCount)
    {
        IncrUse();
    }

    template <typename S>
    YLWeakPtr(const YLSmartPtr<S> & src)
        : ptr(dynamic_cast<T *>(src.ptr)), pRefCount(src.pRefCount)
    {
        IncrUse();
    }

    ~YLWeakPtr()
    {
        DecrUse();
    }

    YLWeakPtr & operator=(const YLWeakPtr & rhs)
    {
        // self-assigning is also right
        // 先增加引用再减少引用，避免自赋值时被释放
        if (rhs.pRefCount)
        {
            ++rhs.pRefCount->nWeakCount;
        }
        DecrUse();
        ptr = rhs.ptr;
        pRefCount = rhs.pRefCount;
        return *this;
    }

    template <typename S>
    YLWeakPtr & operator=(const YLWeakPtr<S> & rhs)
    {
        // self-assigning is also right
        // 先增加引用再减少引用，避免自赋值时被释放
        if (rhs.pRefCount)
        {
            ++rhs.pRefCount->nWeakCount;
        }
        DecrUse();
        ptr = dynamic_cast<T *>(rhs.ptr);
        pRefCount = rhs.pRefCount;
        return *this;
    }

    YLWeakPtr & operator=(const YLSmartPtr<T> & rhs)
    {
        if (rhs.pRefCount)
        {
            ++rhs.pRefCount->nWeakCount;
        }
        DecrUse();
        ptr = rhs.ptr;
        pRefCount = rhs.pRefCount;
        return *this;
    }

    template <typename S>
    YLWeakPtr & operator=(const YLSmartPtr<S> & rhs)
    {
        if (rhs.pRefCount)
        {
            ++rhs.pRefCount->nWeakCount;
        }
        DecrUse();
        ptr = dynamic_cast<T *>(rhs.ptr);
        pRefCount = rhs.pRefCount;
        return *this;
    }

    // 重新关联指针
    void Reset()
    {
        DecrUse();
        ptr = NULL;
        pRefCount = NULL;
    }

    // 判断指针是否为空
    bool IsEmpty() const
    {
        return (pRefCount && pRefCount->nUseCount == 0) || !ptr;
    }

    // 获取共享指针
    YLSmartPtr<T> lock() const
    {
        YLSmartPtr<T> smart;
        if (!IsEmpty())
        {
            smart.ptr = ptr;
            smart.pRefCount = pRefCount;
            smart.IncrUse();
        }
        return smart;
    }

private:
    // 增加引用计数
    void IncrUse()
    {
        if (pRefCount)
        {
            ++pRefCount->nWeakCount;
        }
    }

    // 减少引用计数
    void DecrUse()
    {
        if (pRefCount)
        {
            --pRefCount->nWeakCount;
            if (pRefCount->nWeakCount == 0 && pRefCount->nUseCount == 0)
            {
                delete pRefCount;
                pRefCount = NULL;
            }
        }
    }

private:
    //关联的指针
    T * ptr;
    // 被引用次数
    YLRefCount * pRefCount;
};

namespace yl
{
	template<class T> class enable_shared_from_this
	{
	protected:
		enable_shared_from_this()
		{}

		enable_shared_from_this(enable_shared_from_this const &)
		{}

		enable_shared_from_this & operator=(enable_shared_from_this const &)
		{
			return *this;
		}

		~enable_shared_from_this()
		{}

	public:
		YLSmartPtr<T> shared_from_this()
		{
			return weak_this_.lock();
		}

		YLWeakPtr<T> weak_from_this()
		{
			return weak_this_;
		}

	public: // actually private, but avoids compiler template friendship issues
		// Note: invoked automatically by YLSmartPtr; do not call
		template<class X> void _internal_accept_owner(YLSmartPtr<X> const * ppx) const
		{
			if (weak_this_.IsEmpty())
			{
				weak_this_ = *ppx;
			}
		}

	private:
		mutable YLWeakPtr<T> weak_this_;
	};
}
#endif
