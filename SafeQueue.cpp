// safeQueue.cpp 
//

#include "PCH.h"
#include "SafeQueue.h"

safeQueue::safeQueue()
{
	//CCriticalSection csection;
	//CPtrList list;
	//by Xman94 2009-08-19
	list.RemoveAll();

}

safeQueue::~safeQueue()
{
	while( !list.IsEmpty() )
		list.RemoveHead();
}

BOOL safeQueue::push( LPVOID pData )
{

	csection.Lock();
	list.AddTail( pData );
	csection.Unlock();
	return TRUE;
}

LPVOID safeQueue::pop()
{
	csection.Lock();
	LPVOID pItem = list.RemoveHead();
	csection.Unlock();
	return pItem;
}

int	safeQueue::popAll( CPtrList& list2 )
{
	csection.Lock();
	while( !list.IsEmpty() )
		list2.AddTail( list.RemoveHead() );
	csection.Unlock();
	return list2.GetCount();
}

int	safeQueue::Remove( int iCnt )
{
	int iRet=0;
	if( iCnt <= 0 )
		return 0;

	csection.Lock();
	while( !list.IsEmpty() )
	{
		list.RemoveHead();
		if( ++iRet == iCnt )
			break;
	}
	csection.Unlock();
	return iRet;
}

void safeQueue::RemoveHead()
{
	csection.Lock();
	if( !list.IsEmpty() )
		list.RemoveHead();
	csection.Unlock();
}


LPVOID safeQueue::get()
{
	csection.Lock();
	LPVOID pItem = list.GetHead();
	csection.Unlock();
	return pItem;
}

int	safeQueue::getAll( CPtrList& list2 )
{
	csection.Lock();
	POSITION pos = list.GetHeadPosition();
	while(pos)
	{	
		list2.AddTail(list.GetNext(pos));
	}
	csection.Unlock();
	return list2.GetCount();
}

int safeQueue::getCount()
{
	return list.GetCount();
}