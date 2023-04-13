///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgPtrList
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgPtrList.h,v 1.1.1.1 2002/10/20 09:33:01 daniel Exp $
///////////////////////////////////////////////////////////////////

#ifndef __PTRLIST_HEADERFILE__
#define __PTRLIST_HEADERFILE__

/// Template pointer list, with no refcounting
/// See also: pgList


#include <pgLib/pgLib.h>

#include <assert.h>
#include <string.h>

#include "pgLog.h"

#ifndef NULL
#define NULL    0
#endif

template <class TYPE>
class PGLIB_API pgPtrList
{
public:  
	 pgPtrList();
	 pgPtrList(const pgPtrList<TYPE> &other);
	 virtual 
		~pgPtrList();


	/// Sets how much elements the list grows when growing is necessary
	bool		
		setStandardGrowingSize(int size) const;


	/// Returns the number of objects currently stored in the list
	int			
		getSize() const { return actualItemCount;};


	/// Returns true if the list is empty
	bool		
		isEmpty() const { return actualItemCount==0;};


	/// Checks if the index is valid
	bool		
		checkIndex(int index) const;


	/// Adds one element to the head of the list (SLOW)
	virtual bool		
		addHead(TYPE* item);


	/// Adds one element to the tail of the list
	virtual bool		
		addTail(TYPE* item);


	/// Inserts one element after the given index
	virtual bool		
		insertAfter(int index,TYPE* item);


	/// Inserts one element before the given index
	virtual bool		
		insertBefore(int index,TYPE* item);


	/// Overwrites one element
	virtual bool		
		setAt(int index,TYPE* item);


	/// Replaces the indices of the 2 items
	virtual bool		
		replaceIndices(int nIndexA,int nIndexB);


	/// Replaces the first found element which is equal to 'old'  with 'item'
	virtual bool		
		replace(TYPE* old,TYPE* item);


	/// Replaces all elements which are equal to 'old' with 'item'
	virtual bool		
		replaceAll(TYPE* old,TYPE* item);


	/// Returns the first element of the list
	TYPE*		
		getHead();


	/// Returns the last element of the list
	TYPE*		
		getTail();


	/// Returns the element at the given position
	TYPE*		
		getAt(int index) const;


	/// Removing the first element of the list (SLOW)
	bool		
		removeHead();


	/// Removing the last element of the list
	bool		
		removeTail();


	/// Removes the first element 'item'
	bool		
		removeItem(TYPE* item);


	/// Removes the element a position 'index'
	virtual bool		
		removeIndex(int index);


	/// Empties the list.
	virtual bool		
		removeAll();


	/// Finds the first element which is equal to 'item'
	int			
		find(const TYPE* item);


	/// Finds the first element which is equal to 'item' and positioned after 'after'
	int			
		findAfter(const TYPE* item,int after);


	/// Finds the first element which is equal to 'item' and positioned before 'before'
	int			
		findBefore(const TYPE* item,int before);


	/// Returns a part of the list
	virtual pgPtrList*	
		getSubList(int from,int to);


	/// Gives direct access to the list members
    TYPE* operator[](int index) const 
		{ return getAt(index); }


	/// Copies a list from another list
    virtual inline pgPtrList<TYPE>&
		operator=(const pgPtrList<TYPE>& other);


	/// Adds the elements of another list to this list
    virtual inline pgPtrList<TYPE>&
		operator+=(const pgPtrList<TYPE>& other);

protected:
	static int	standardGrowingSize;
	int			actualBlockSize;
	int			actualItemCount;
	TYPE**		array;

	bool		enlargeList(int size=-1);
	bool		reserveBlockSize(int size);

	bool //raises an error
		raiseError(const char* nError) const
			{  pgLog::error(nError);  return false;  }
	//plString	error;	
};

template <class TYPE>
 int pgPtrList<TYPE>::standardGrowingSize = 16;


template <class TYPE> inline
pgPtrList<TYPE>::pgPtrList<TYPE>()
{
	array				= NULL;
	actualBlockSize	= 0;
	actualItemCount	= 0;

	enlargeList();
}

template <class TYPE> inline
pgPtrList<TYPE>::pgPtrList<TYPE>(const pgPtrList<TYPE> &other)
{
	//pgPtrList<TYPE>();
	array				= NULL;
	actualBlockSize	= 0;
	actualItemCount	= 0;

	enlargeList();

	*this = other;
}

template <class TYPE> inline
pgPtrList<TYPE>::~pgPtrList()
{
	bool ret = removeAll();
	ret;
	assert(ret);
	if (array!=NULL)
		delete []array;
	array = NULL;
}

template <class TYPE> inline bool 
pgPtrList<TYPE>::enlargeList(int size)
{
	TYPE** tempList;
	if (size == -1)
		size = standardGrowingSize;
	try
	{
		tempList = new TYPE*[actualBlockSize + size];
		int i;
		for (i=0; i < actualBlockSize;++i)
			tempList[i] = array[i];
		for (; i < ( actualBlockSize+size );++i)
			tempList[i] = NULL;
		if (array!=NULL)
			delete array;
		array = tempList;
		actualBlockSize += size;
	}
	catch(...)
	{
//		return RaiseError("in EnlargeList");
	}
	return true;
}

template <class TYPE> inline bool 
pgPtrList<TYPE>::setStandardGrowingSize(int size) const
{
	if (size<1)
		return raiseError("Wrong Size");
	standardGrowingSize = size;
	return true;
}

template <class TYPE> inline bool 
pgPtrList<TYPE>::reserveBlockSize(int size)
{
	if (actualBlockSize < actualItemCount+size)
		return enlargeList();
	return true;
}

template <class TYPE> inline bool 
pgPtrList<TYPE>::checkIndex(int index) const
{
	if (index<0 || index > actualItemCount-1)
		return false;
	return true;
}

template <class TYPE> inline bool 
pgPtrList<TYPE>::addHead(TYPE* item)
{
	if (!reserveBlockSize(1))
		return false;
	try
	{	
		memmove((void*)(reinterpret_cast<unsigned>(array)+sizeof(TYPE*)),
				array,actualItemCount*sizeof(TYPE*));
		array[0] = item;
		++actualItemCount;
	}
	catch(...)
	{
//		return RaiseError("in AddHead");
	}
	return true;
}

template <class TYPE> inline bool 
pgPtrList<TYPE>::addTail(TYPE* item)
{
	if (!reserveBlockSize(1))
		return false;
	try
	{
		array[actualItemCount] = item;
		++actualItemCount;
		
	}
	catch(...)
	{
//		return RaiseError("in AddTail");
	}
	return true;
}

template <class TYPE> inline bool 
pgPtrList<TYPE>::insertAfter(int index,TYPE* item)
{
	if (!checkIndex(index))
		return raiseError("IndexError");
	if (index == actualItemCount-1)
		return addTail(item);
	else
	{
		if (!reserveBlockSize(1))
			return false;
		try
		{
			memmove((void*)(reinterpret_cast<unsigned>(array)+(index+2)*sizeof(TYPE*)),
					(void*)(reinterpret_cast<unsigned>(array)+(index+1)*sizeof(TYPE*)),
							(actualItemCount-index-1)*sizeof(TYPE*));
			array[index+1] = item;
			++actualItemCount;
		}
		catch(...)
		{
//			return RaiseError("in InsertAfter");
		}
	}
	return true;
}

template <class TYPE> inline bool 
pgPtrList<TYPE>::insertBefore(int index,TYPE* item)
{
	if (!checkIndex(index))
		return raiseError("IndexError");
	if (index == 0)
		return addHead(item);
	else
	{
		if (!reserveBlockSize(1))
			return false;
		try
		{
			memmove((void*)(reinterpret_cast<unsigned>(array)+(index+1)*sizeof(TYPE*)),
					(void*)(reinterpret_cast<unsigned>(array)+(index)*sizeof(TYPE*)),
							(actualItemCount-index)*sizeof(TYPE*));
			array[index] = item;
			++actualItemCount;
		}
		catch(...)
		{
			return raiseError("in InsertBefore");
		}
	}
	return true;
}

template <class TYPE> inline bool 
pgPtrList<TYPE>::setAt(int index,TYPE* item)
{
	if (!checkIndex(index))
		return raiseError("IndexError");
	try
	{
		array[index] = item;
	}
	catch(...)
	{
//		return RaiseError("in SetAt");
	}
	return true;
}

template <class TYPE> inline bool		
pgPtrList<TYPE>::replaceIndices(int nIndexA,int nIndexB)
{
	if (!checkIndex(nIndexA))
		return false;
	if (!checkIndex(nIndexB))
		return false;
	TYPE *item = array[nIndexA];
	array[nIndexA] = array[nIndexB];
	array[nIndexB] = item;
	return true;
}

template <class TYPE> inline bool 
pgPtrList<TYPE>::replace(TYPE* old,TYPE* item)
{
	bool ret = false;
	for (int i=0; i < actualItemCount; ++i)
		if (array[i] == old)
		{
			array[i] = item;
			ret = true;
			break;
		}
	return ret;
}

template <class TYPE> inline bool 
pgPtrList<TYPE>::replaceAll(TYPE* old,TYPE* item)
{
	bool ret = false;
	for (int i=0; i < actualItemCount; ++i)
		if (array[i] == old)
		{
			array[i] = item;
			ret = true;
		}
	return ret;
}

template <class TYPE> inline TYPE* 
pgPtrList<TYPE>::getHead()
{
	TYPE* ret = NULL;
	if (actualItemCount>0)
	{
//		RaiseError("Item not found");
		return NULL;
	}
	try
	{
		ret = array[0];
	}
	catch(...)
	{
//		RaiseError("in GetHead");
		return NULL;
	}
	return ret;
}

template <class TYPE> inline TYPE* 
pgPtrList<TYPE>::getTail()
{
	TYPE* ret = NULL;
	if (actualItemCount<1)
		return NULL;
	try
	{
		ret = array[actualItemCount-1];
	}
	catch(...)
	{
//		RaiseError("in GetTail");
		return NULL;
	}
	return ret;
}

template <class TYPE> inline TYPE* 
pgPtrList<TYPE>::getAt(int index) const
{
	TYPE* ret = NULL;
	if (!checkIndex(index))
	{
//		RaiseError("IndexError");
		return NULL;
	}
	try
	{
		ret = array[index];
		assert(ret!=NULL);
	}
	catch(...)
	{
//		RaiseError("in Get At");
		return NULL;
	}
	return ret;
}

template <class TYPE> inline bool 
pgPtrList<TYPE>::removeHead()
{
	return removeIndex(0);
}

template <class TYPE> inline bool 
pgPtrList<TYPE>::removeTail()
{
	return removeIndex(actualItemCount-1);
}

template <class TYPE> inline bool 
pgPtrList<TYPE>::removeItem(TYPE* item)
{
	int index;
	index = find(item);
	if (index < 0)
		return false;
	return removeIndex(index);
}

template <class TYPE> inline bool 
pgPtrList<TYPE>::removeIndex(int index)
{
	if (!checkIndex(index))
		return raiseError("IndexError");
	try
	{
		memmove((void*)(reinterpret_cast<unsigned>(array)+(index)*sizeof(TYPE*)),
				(void*)(reinterpret_cast<unsigned>(array)+(index+1)*sizeof(TYPE*)),
						(actualItemCount-index-1)*sizeof(TYPE*));
		--actualItemCount;
	}
	catch(...)
	{
//		return RaiseError("in RemoveIndex");
	}
	return true;
}

template <class TYPE> inline bool 
pgPtrList<TYPE>::removeAll()
{
	try
	{
		actualItemCount = 0;
	}
	catch(...)
	{
//		return RaiseError("in RemoveAll");
	}
	return true;
}

template <class TYPE> inline int 
pgPtrList<TYPE>::find(const TYPE* item)
{
	int ret = -1;
	try
	{
		for (int i=0; i < actualItemCount; ++i)
			if (item == array[i])
			{
				ret = i;
				break;
			}
	}
	catch(...)
	{
//		return RaiseError("in Find");
	}
	return ret;
}

template <class TYPE> inline int 
pgPtrList<TYPE>::findAfter(const TYPE* item,int after)
{
	int ret = -1;
	int index = after+1;
	if (!checkIndex(index))
		return raiseError("IndexError");
	try
	{
		for (int i=index; i < actualItemCount; ++i)
			if (item == array[i])
			{
				ret = i;
				break;
			}
	}
	catch(...)
	{
//		return RaiseError("in FindAfter");
	}
	return ret;
}

template <class TYPE> inline int 
pgPtrList<TYPE>::findBefore(const TYPE* item,int before)
{
	int ret = -1;
	int index = before-1;
	if (!checkIndex(index))
		return raiseError("IndexError");
	try
	{
		for (int i=0; i < before; ++i)
			if (item == array[i])
			{
				ret = i;
				break;
			}
	}
	catch(...)
	{
//		return RaiseError("in FindBefore");
	}
	return ret;
}

template <class TYPE> inline pgPtrList<TYPE>* 
pgPtrList<TYPE>::getSubList(int from,int to)
{
	if (!checkIndex(from))
	{
//		RaiseError("IndexError");
		return NULL;
	}
	if (!checkIndex(to))
	{
//		RaiseError("IndexError");
		return NULL;
	}
	if (from < to)
	{
//		RaiseError("IndexError");
		return NULL;
	}
	pgPtrList<TYPE> *ret = new pgPtrList<TYPE>;
	try
	{
		for (int i=from; i < to; ++i)
			ret->addTail(array[i]);
	}
	catch(...)
	{
		delete ret;
//		RaiseError("in GetSubList");
		return NULL;
	}
	return ret;
}

template <class TYPE> inline pgPtrList<TYPE>&
pgPtrList<TYPE>::operator=(const pgPtrList<TYPE>& other)
{
	removeAll();
	for (int i=0;i<other.getSize();++i)
		addTail(other[i]);
	return *this;
}

template <class TYPE> inline pgPtrList<TYPE>&
pgPtrList<TYPE>::operator+=(const pgPtrList<TYPE>& other)
{
	for (int i=0;i<other.getSize();++i)
		addTail(other[i]);
	return *this;
}

#endif //__PTRLIST_HEADERFILE__
