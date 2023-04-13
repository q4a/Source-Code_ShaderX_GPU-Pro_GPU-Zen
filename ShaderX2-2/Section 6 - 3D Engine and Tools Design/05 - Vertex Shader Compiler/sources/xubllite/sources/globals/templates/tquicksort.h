#ifndef __QUICKSORT__
#define __QUICKSORT__


const int minForQSort = 5;

template<class T>

void TQuickSortInc( T *a, int num_el )
{
int s_pos=0;
T *first = a, *last = a + (num_el - 1);
T *stack[32*2]; 

T temp;

do
{
do
{
if (last-first <= minForQSort)
{


T *lo = first, *hi = last;
while(hi > lo)
{
T *max = lo;
for( T *p = lo+1; p <= hi; p++ )
{
if(!(p[0] < max[0])) max = p;  
}
temp = max[0]; max[0] = hi[0];  hi[0] = temp;  

hi--;
}

break;
}



T *mid = (last - first >> 1) + first;
temp = first[0]; first[0] = mid[0];  mid[0] = temp;  


T *cfirst = first,
*clast = last + 1;



do
{
do
{
cfirst++;
}
while( cfirst <= last && cfirst[0] < first[0] ); 

do
{
clast--;
}
while( clast > first && !(clast[0] < first[0] )); 

if (clast < cfirst) break;
temp = cfirst[0]; cfirst[0] = clast[0];  clast[0] = temp;  
}
while(1);

temp = first[0]; first[0] = clast[0];  clast[0] = temp;  



if( clast-1 - first >= last - cfirst )
{
if( first + 1 < clast )
{
stack[ s_pos ] = first;
stack[ s_pos+1 ] = clast - 1;
s_pos += 2;
}

if( cfirst < last )
{
first = cfirst;
}
else break;
}
else
{
if( cfirst < last )
{
stack[ s_pos ] = cfirst;
stack[ s_pos+1 ] = last;
s_pos += 2;
}

if( first + 1 < clast )
{
last = clast - 1;
}
else break;
}
}
while(1);



if( !s_pos ) break;
s_pos -=2;  first = stack[ s_pos ];  last = stack[ s_pos+1 ];
}
while(1);
}























template<class T>
void TQuickSortDec( T *a, int num_el )
{
int s_pos=0;
T *first = a, *last = a + (num_el - 1);
T *stack[32*2]; 

T temp;

do
{
do
{
if (last-first <= minForQSort)
{


T *lo = first, *hi = last;
while(hi > lo)
{
T *max = lo;
for( T *p = lo+1; p <= hi; p++ )
{
if(p[0] < max[0]) max = p;  
}
temp = max[0]; max[0] = hi[0];  hi[0] = temp;  

hi--;
}

break;
}



T *mid = (last - first >> 1) + first;
temp = first[0]; first[0] = mid[0];  mid[0] = temp;  


T *cfirst = first,
*clast = last + 1;



do
{
do
{
cfirst++;
}
while( cfirst <= last && !(cfirst[0] < first[0] )); 

do
{
clast--;
}
while( clast > first && clast[0] < first[0] ); 

if (clast < cfirst) break;
temp = cfirst[0]; cfirst[0] = clast[0];  clast[0] = temp;  
}
while(1);

temp = first[0]; first[0] = clast[0];  clast[0] = temp;  



if( clast-1 - first >= last - cfirst )
{
if( first + 1 < clast )
{
stack[ s_pos ] = first;
stack[ s_pos+1 ] = clast - 1;
s_pos += 2;
}

if( cfirst < last )
{
first = cfirst;
}
else break;
}
else
{
if( cfirst < last )
{
stack[ s_pos ] = cfirst;
stack[ s_pos+1 ] = last;
s_pos += 2;
}

if( first + 1 < clast )
{
last = clast - 1;
}
else break;
}
}
while(1);



if( !s_pos ) break;
s_pos -=2;  first = stack[ s_pos ];  last = stack[ s_pos+1 ];
}
while(1);
}





#endif 
