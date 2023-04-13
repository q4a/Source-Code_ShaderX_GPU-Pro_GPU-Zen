///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgList
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgList.h,v 1.3 2002/12/15 20:04:54 daniel Exp $
///////////////////////////////////////////////////////////////////

#ifndef __LIST_HEADERFILE__
#define __LIST_HEADERFILE__


#include <pgLib/pgLib.h>

#include <assert.h>
#include <string.h>

#include "pgLog.h"


/// pgList is a template class for storing simple objects directly in the list (no pointers)
/**
 *  See also: plPtrList
 */
#pragma warning( disable: 4514 )

template <class TYPE>
class PGLIB_API pgList
{
public:   
	pgList();
	pgList(const pgList<TYPE> &other);

	 virtual 
		~pgList();

	/// Returns the number of objects currently stored in the list
	int			
		getSize() const  { return actualItemCount; }


	/// Returns true if the list is empty
	bool		
		isEmpty() const  { return actualItemCount==0; }


	/// Checks if the index is valid
	bool		
		checkIndex(int index) const;


	/// Sets how many elements the list grows when growing is done
	void
		setGrowingSize(int size)  { growingSize = size; }


	/// Lets the list grow 'size' elemens
	bool
		enlargeList(int size=-1);


	/// Enlarge the lists by a given size and set the lists size
	bool
		enlargeListSet(int size=-1);


	/// Enlarges/Reduces the list to a given size.
	int			
		setSize(int nSize);


	/// Adds one element to the head of the list (SLOW)
	virtual int		
		addHead(const TYPE& item);


	/// Adds one element to the tail of the list
	virtual int		
		addTail(const TYPE& item);


	/// Adds the last element of the list to the list again
	virtual int		
		addTailAgain();


	/// Adds 'nSize' elements to the tail of the list
	virtual int		
		addTail(const TYPE* item,int nSize);


	/// Inserts one element after the given index (SLOW)
	virtual int		
		insertAfter(int index,const TYPE& item);


	/// Inserts one element before the given index (SLOW)
	virtual int		
		insertBefore(int index,const TYPE& item);


	/// Takes 'nSize' elements starting at the given address and copies them into the list
	virtual bool		
		setList(const TYPE* item,int nSize);


	/// Overwrites one element
	virtual bool		
		setAt(int index,const TYPE& item);


	/// Replaces the indices of the 2 items
	virtual bool		
		replaceIndices(int nIndexA,int nIndexB);


	/// Replaces the first found element which is equal to 'old'  with 'item'
	virtual bool		
		replace(const TYPE& old,const TYPE& item);


	/// Replaces all elements which are equal to 'old' with 'item'
	virtual bool		
		replaceAll(const TYPE& old,const TYPE& item);


	/// Returns the first element of the list
	/**
	 *  Returns false if the list is empty
	 */
	bool		
		getHead(TYPE& item) const;


	/// Returns the first element of the list
	TYPE&
		getHead() const;


	/// Returns the last element of the list
	/**
	 *  Returns false if the list is empty
	 */
	bool		
		getTail(TYPE& item) const;


	/// Returns the last element of the list
	TYPE&
		getTail() const;


	/// Returns the element at the given position
	/**
	 *  Returns false if the list is empty
	 */
	bool		
		getAt(int index,TYPE& item) const;


	/// Returns the element at the given position
	TYPE&
		getAt(int index) const;


	/// Removing the first element of the list (SLOW)
	bool		
		removeHead();


	/// Removing the last element of the list
	bool		
		removeTail();


	/// Removes the first element in the list which is equal to 'item'
	bool
		removeItem(const TYPE& item);


	/// Removes the element a position 'index'
	virtual bool
		removeIndex(int index);


	/// Empties the list.
	virtual bool		
		removeAll();


	/// Finds the first element which is equal to 'item'
	int			
		find(const TYPE& item);


	/// Finds the first element which is equal to 'item' and positioned after 'after'
	int			
		findAfter(const TYPE& item,int after);


	/// Finds the first element which is equal to 'item' and positioned before 'before'
	int			
		findBefore(const TYPE& item,int before);


	/// Returns a part of the list
	virtual pgList*	
		getSubList(int from,int to);

	/// Gives direct access to the list members
    TYPE& 
		operator[](unsigned index) const { return getAt(index); }


	/// Copies a list from another list
    inline pgList<TYPE>&
		operator=(const pgList<TYPE>& other);


	/// Compares two lists
	/**
	 *  Each item is compared. (therefore TYPE must have an operator==).
	 *  If all items are ident, true is returned.
	 */
	friend inline bool 
		operator==(const pgList&, const pgList&)  { return false; }


protected:
	int			growingSize;
	int			actualBlockSize;
	int			actualItemCount;
	TYPE*		array;

	bool		reserveBlockSize(int size);
protected:
	bool //raises an error
		raiseError(const char* nError) const
		{  pgLog::error(nError);  return false;  }
};


template <class TYPE> inline
pgList<TYPE>::pgList<TYPE>()
{
	array			= NULL;
	actualBlockSize	= 0;
	actualItemCount	= 0;
	growingSize		= 16;

	enlargeList();
}

template <class TYPE> inline
pgList<TYPE>::pgList<TYPE>(const pgList<TYPE> &other)
{
	pgList<TYPE>();
	*this = other;
}

template <class TYPE> inline
pgList<TYPE>::~pgList()
{
	bool ret = removeAll();
	ret;
	if(array)
		delete [] array;
	assert(ret);
}

template <class TYPE> inline bool 
pgList<TYPE>::enlargeList(int size)
{
	TYPE* tempList;
	if (size == -1)
		size = growingSize;
	try
	{
		tempList = new TYPE[actualBlockSize + size];
//		memcpy(tempList,array,actualBlockSize*sizeof(TYPE));
		for (int i=0; i < actualItemCount;++i)
			tempList[i] = array[i];
		//for (; i < ( actualBlockSize+size );++i)
		//	tempList[i] = NULL;
		if (array!=NULL)
			delete [] array;
		array = tempList;

		actualBlockSize += size;
	}
	catch(...)
	{
		return raiseError("in enlargeList");
	}
	return true;
}

template <class TYPE> inline bool 
pgList<TYPE>::enlargeListSet(int size)
{
	if (enlargeList(size))
	{
		actualItemCount = actualBlockSize;
		return true;
	}
	return false;
}

template <class TYPE> inline int			
pgList<TYPE>::setSize(int nSize)
{
	if(array!=NULL)
		delete [] array;
	int size;
	if (nSize<1)
		size = growingSize;
	else
		size = (nSize-(nSize%growingSize)) + growingSize;
	array = new TYPE[size];
	actualItemCount = nSize;
	return actualItemCount;
}

template <class TYPE> inline bool 
pgList<TYPE>::reserveBlockSize(int size)
{
	if (actualBlockSize < actualItemCount+size)
		return enlargeList();
	return true;
}

template <class TYPE> inline bool 
pgList<TYPE>::checkIndex(int index) const
{
	if (index<0 || index > actualItemCount-1)
		return false;
	return true;
}

template <class TYPE> inline int
pgList<TYPE>::addHead(const TYPE& item)
{
	if (!reserveBlockSize(1))
		return -1;
	try
	{	
		memmove((void*)(reinterpret_cast<unsigned>(array)+sizeof(TYPE)),
				array,actualItemCount*sizeof(TYPE));
		array[0] = item;
		++actualItemCount;
	}
	catch(...)
	{
		return raiseError("in addHead");
	}
	return 0;
}

template <class TYPE> inline int 
pgList<TYPE>::addTail(const TYPE& item)
{ 
	if (!reserveBlockSize(1)) 
		return -1;
	try
	{
		array[actualItemCount] = item;
		++actualItemCount;
		
	}
	catch(...)
	{
		return raiseError("in addTail");
	}
	return (actualItemCount-1);
}

template <class TYPE> inline int 
pgList<TYPE>::addTail(const TYPE* item,int nSize)
{ 
	if (!reserveBlockSize(nSize)) 
		return -1;
	try
	{
		for (int i=0;i<nSize;++i)
			array[actualItemCount+i] = item[i];
		actualItemCount += nSize;
	}
	catch(...)
	{
		return raiseError("in addTail");
	}
	return (actualItemCount-1);
}


template <class TYPE> inline int 
pgList<TYPE>::addTailAgain()
{
	if(actualItemCount>0)
	{
		TYPE item = array[actualItemCount-1];		// need to copy since a list grow could screw the reference
		return addTail(item);
	}
	else
		return -1;
}


template <class TYPE> inline int 
pgList<TYPE>::insertAfter(int index,const TYPE& item)
{
	if (!checkIndex(index))
	{
		raiseError("IndexError");
		return -1;
	}
	if (index == actualItemCount-1)
		return addTail(item);
	else
	{
		if (!reserveBlockSize(1))
			return -1;
		try
		{
			memmove((void*)(reinterpret_cast<unsigned>(array)+(index+2)*sizeof(TYPE)),
					(void*)(reinterpret_cast<unsigned>(array)+(index+1)*sizeof(TYPE)),
							(actualItemCount-index-1)*sizeof(TYPE));
			array[index+1] = item;
			++actualItemCount;
		}
		catch(...)
		{
			raiseError("in InsertAfter");
			return -1;
		}
	}
	return (index+1);
}

template <class TYPE> inline int 
pgList<TYPE>::insertBefore(int index,const TYPE& item)
{
	if (!checkIndex(index))
	{
		raiseError("IndexError");
		return -1;
	}
	if (index == 0)
		return addHead(item);
	else
	{
		if (!reserveBlockSize(1))
			return -1;
		try
		{
			memmove((void*)(reinterpret_cast<unsigned>(array)+(index+1)*sizeof(TYPE)),
					(void*)(reinterpret_cast<unsigned>(array)+(index)*sizeof(TYPE)),
							(actualItemCount-index)*sizeof(TYPE));
			array[index] = item;
			++actualItemCount;
		}
		catch(...)
		{
			raiseError("in InsertBefore");
			return -1;
		}
	}
	return (index-1);
}

template <class TYPE> inline bool 
pgList<TYPE>::setList(const TYPE* item,int nSize)
{
	if(array!=NULL)
		delete [] array;
	int size;
	if (nSize<1)
		size = growingSize;
	else
		size = (nSize-(nSize%growingSize)) + growingSize;
	array = new TYPE[size];
	memmove((void*)reinterpret_cast<unsigned>(array),
			(void*)reinterpret_cast<unsigned>(item),
					(nSize)*sizeof(TYPE));
	actualItemCount = nSize;
	return true;
}

template <class TYPE> inline bool 
pgList<TYPE>::setAt(int index,const TYPE& item)
{
	if (!checkIndex(index))
		return raiseError("IndexError");
	try
	{
		array[index] = item;
	}
	catch(...)
	{
		return raiseError("in SetAt");
	}
	return true;
}

template <class TYPE> inline bool		
pgList<TYPE>::replaceIndices(int nIndexA,int nIndexB)
{
	if (!checkIndex(nIndexA))
		return false;
	if (!checkIndex(nIndexB))
		return false;
	TYPE item = array[nIndexA];
	array[nIndexA] = array[nIndexB];
	array[nIndexB] = item;
	return true;
}

template <class TYPE> inline bool 
pgList<TYPE>::replace(const TYPE& old,const TYPE& item)
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
pgList<TYPE>::replaceAll(const TYPE& old,const TYPE& item)
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

template <class TYPE> inline bool 
pgList<TYPE>::getHead(TYPE& item) const
{
	if (actualItemCount>0)
	{
		raiseError("Item not found");
		return false;
	}
	try
	{
		item = array[0];
	}
	catch(...)
	{
		raiseError("in GetHead");
		return false;
	}
	return true;
}

template <class TYPE> inline TYPE&
pgList<TYPE>::getHead() const
{
	if(actualItemCount<1)
		raiseError("Item not found");

	try
	{
		return array[0];
	}
	catch(...)
	{
		raiseError("in GetHead");
	}
}

template <class TYPE> inline bool 
pgList<TYPE>::getTail(TYPE& item) const
{
	if (actualItemCount<1)
	{
		raiseError("Item not found");
		return false;
	}
	try
	{
		item = array[actualItemCount-1];
	}
	catch(...)
	{
		raiseError("in GetTail");
		return false;
	}
	return true;
}

template <class TYPE> inline TYPE&
pgList<TYPE>::getTail()  const
{
	if(actualItemCount<1)
		raiseError("Item not found");

	try
	{
		return array[actualItemCount-1];
	}
	catch(...)
	{
		raiseError("in GetTail");
	}
}

template <class TYPE> inline bool
pgList<TYPE>::getAt(int index,TYPE& item) const
{
	if (!checkIndex(index))
	{
		raiseError("IndexError");
		assert(1);
		return false;
	}
	try
	{
		item = array[index];
	}
	catch(...)
	{
		raiseError("in Get At");
		return false;
	}
	return true;
}

template <class TYPE> inline TYPE&
pgList<TYPE>::getAt(int index) const
{
	if (!checkIndex(index))
		raiseError("IndexError");

	try
	{
		return array[index];
	}
	catch(...)
	{
		raiseError("in Get At");
	}
	//will never occure because of raiseError
	//it just prevent a compiler-warning
	return array[index];
}

template <class TYPE> inline bool 
pgList<TYPE>::removeHead()
{
	return removeIndex(0);
}

template <class TYPE> inline bool 
pgList<TYPE>::removeTail()
{
	//return removeIndex(actualItemCount-1);
	if(actualItemCount>0)
	{
		actualItemCount--;
		return true;
	}
	else
		return raiseError("in removeTail");
}

template <class TYPE> inline bool 
pgList<TYPE>::removeItem(const TYPE& item)
{
	int index;
	index = find(item);
	if (index < 0)
		return false;
	return removeIndex(index);
}

template <class TYPE> inline bool 
pgList<TYPE>::removeIndex(int index)
{
	if (!checkIndex(index))
		return raiseError("IndexError");
	try
	{
		memmove((void*)(reinterpret_cast<unsigned>(array)+(index)*sizeof(TYPE)),
				(void*)(reinterpret_cast<unsigned>(array)+(index+1)*sizeof(TYPE)),
						(actualItemCount-index-1)*sizeof(TYPE));
		--actualItemCount;
	}
	catch(...)
	{
		return raiseError("in removeIndex");
	}
	return true;
}

template <class TYPE> inline bool 
pgList<TYPE>::removeAll()
{
	try
	{
		actualItemCount = 0;
	}
	catch(...)
	{
		return raiseError("in RemoveAll");
	}
	return true;
}

template <class TYPE> inline int 
pgList<TYPE>::find(const TYPE& item)
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
		return raiseError("in Find");
	}
	return ret;
}

template <class TYPE> inline int 
pgList<TYPE>::findAfter(const TYPE& item,int after)
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
		return raiseError("in FindAfter");
	}
	return ret;
}

template <class TYPE> inline int 
pgList<TYPE>::findBefore(const TYPE& item,int before)
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
		return raiseError("in FindBefore");
	}
	return ret;
}

template <class TYPE> inline pgList<TYPE>* 
pgList<TYPE>::getSubList(int from,int to)
{
	if (!checkIndex(from))
	{
		raiseError("IndexError");
		return NULL;
	}
	if (!checkIndex(to))
	{
		raiseError("IndexError");
		return NULL;
	}
	if (from < to)
	{
		raiseError("IndexError");
		return NULL;
	}
	pgList<TYPE> *ret = new pgList<TYPE>;
	try
	{
		for (int i=from; i < to; ++i)
			ret->addTail(array[i]);
	}
	catch(...)
	{
		delete ret;
		raiseError("in GetSubList");
		return NULL;
	}
	return ret;
}


template <class TYPE> inline pgList<TYPE>&
pgList<TYPE>::operator=(const pgList<TYPE>& other)
{
	removeAll();
	if (array!=NULL)
		delete [] array;
	if (other.array == NULL || other.actualItemCount==0)
	{
		array = NULL;
		actualBlockSize = 0;
		actualItemCount = 0;
		enlargeList();
	}
	else
	{
		array = new TYPE[other.actualItemCount];
		memmove((void*)(reinterpret_cast<unsigned>(array)),
				(void*)(reinterpret_cast<unsigned>(other.array)),
						(other.actualItemCount)*sizeof(TYPE));
		actualItemCount = other.actualItemCount;
		actualBlockSize = actualItemCount;
	}
/*
	for (int i=0;i<other.GetSize();++i)
		AddTail(other[i]);
*/
	return *this;
}


typedef pgList<int> pgIntList;


#endif //__LIST_HEADERFILE__
