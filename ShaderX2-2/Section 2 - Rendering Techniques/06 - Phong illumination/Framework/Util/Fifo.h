                  /*--.          .-"-. 
                 /   ._.        / ´ ` \
                 \_  (__\       \_°v°_/
                 //   \\        //   \\
                ((     ))      ((     ))
 ¤¤¤¤¤¤¤¤¤¤¤¤¤¤--""---""--¤¤¤¤--""---""--¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 ¤                 |||            |||                                               ¤
 ¤  Author's note:  |              |                                                ¤
 ¤                                                                                  ¤
 ¤  This file is part of work done by Emil Persson, aka Humus.                      ¤
 ¤  You're free to use the code in any way you like, modified, unmodified           ¤
 ¤  or cut'n'pasted into your own work. But for the good of the whole               ¤
 ¤  programming community I will ask you to as long as possible and to the          ¤
 ¤  extent you find suitable to follow these points:                                ¤
 ¤   * If you use this file and it's contents unmodified, please leave this note.   ¤
 ¤   * If you use a major part of the file in your app, please credit the author(s) ¤
 ¤     in one way or the other as you find suitable, for instance a small notice    ¤
 ¤     in the readme file may do, or perhaps a link to the authors site.            ¤
 ¤   * Share your work and ideas too as much as you can.                            ¤
 ¤    _______                                                                       ¤
 ¤  /` _____ `\;,    Humus                                                          ¤
 ¤ (__(^===^)__)';,  emiper-8@student.luth.se                         ___           ¤
 ¤   /  :::  \   ,;  ICQ #47010716                                  /^   ^\         ¤
 ¤  |   :::   | ,;'                                                ( Ö   Ö )        ¤
 ¤¤¤'._______.'`¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤--°oOo--(_)--oOo°--¤¤*/

#ifndef _FIFO_H_
#define _FIFO_H_

#ifndef NULL
#define NULL 0
#endif

template <class ARG_TYPE>
class FifoNode {
public:
	ARG_TYPE object;
	FifoNode *next;
};

template <class ARG_TYPE>
class Fifo {
protected:
	unsigned int size;
	FifoNode <ARG_TYPE> *first, *last;
public:
	Fifo(){
		size = 0;
		first = last = NULL;
	}

	~Fifo(){
		clear();
	}

	void clear(){
		while (dequeue());
	}

	const unsigned int getSize() const { return size; }

	const ARG_TYPE &getFirst(){ return first->object; }
	const ARG_TYPE &getLast() { return last->object;  }

	void enqueue(const ARG_TYPE &object){
		FifoNode <ARG_TYPE> *node = new FifoNode <ARG_TYPE>;
		node->object = object;
		node->next = NULL;

		if (size){
			last->next = node;
			last = node;
		} else {
			first = last = node;
		}

		size++;
	}

	bool dequeue(){
		if (size){
			FifoNode <ARG_TYPE> *tmp = first;

			first = first->next;
			delete tmp;

			size--;
			return true;
		}
		return false;
	}

};

#endif // _FIFO_H_
