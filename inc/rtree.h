#ifndef __RTREE_H__
#define __RTREE_H__

#include <cmath>
#include <cassert>

// 현재 버전은 메모리 풀을 사용하지 않는다. 메모리풀을 사용하기 위해서는
// 이 매크로가 선언되어 있는 부분을 찾아서 수정해야 한다.
#define RTREE_DONT_USE_MEMPOOLS 

// 노드의 분할을 위해서 '구' 형식의 부피를 사용한다. 
// 노드를 좀 더 잘 분할할 수 있으나, 속도가 약간 느릴 수 있다.
#define RTREE_USE_SPHERICAL_VOLUME 

// 파일 IO 헬퍼 클래스
class RTFileStream; 

//////////////////////////////////////////////////////////////////////////////
/// \class CRTree
/// \brief N 차원의 바운딩 박스 트리.
///
/// DATATYPE     유저 데이터 형식. int, void*, obj* 같은 형식만 사용 가능.
///              즉 int 보다 사이즈가 큰 타입은 사용할 수 없다.
/// ELEMTYPE     좌표계를 나타내는 데 사용할 타입. int, float 등등
/// NUMDIMS      차원의 수. 2, 3 등등
/// ELEMTYPEREAL 내부적으로 부피 등을 계산하는 데 쓰일 타입. 정수형보다는
///              실수형의 타입을 사용해야 한다.
/// TMAXNODES    한 노드에 들어갈 수 있는 자식 노드의 최대 갯수
/// TMINNODES    한 노드에 들어가야하는 자식 노드의 최소 갯수
///
/// 3차원의 트리를 구성하기 위해서는 CRTree<Object*, float, 3>과 같은 형식으로
/// 사용하면 된다.
///
/// \note 유저 데이터를 집어넣고 삭제하기 위해서는 그 데이터를 감싸는 최소의
/// 바운딩 박스(Minimal Bounding Box)를 알아야 한다. 
/// \note 현재 버전은 노드의 할당/삭제를 위해 그냥 new/delete를 사용한다.
/// 효율을 위해서는 메모리 풀을 이용하는 것이 좋을 것이다.
//////////////////////////////////////////////////////////////////////////////

template <class DATATYPE, class ELEMTYPE, int NUMDIMS, 
    class ELEMTYPEREAL = ELEMTYPE, int TMAXNODES = 8, int TMINNODES = TMAXNODES / 2>
class CRTree
{
protected: 
    struct Node;


public:
    enum
    {
        MAXNODES = TMAXNODES, ///< 한 노드에 들어갈 수 있는 자식 노드의 최대 갯수
        MINNODES = TMINNODES  ///< 한 노드에 들어가야하는 자식 노드의 최소 갯수
    };

    // 결과값을 반환하기 위한 벡터
    typedef std::vector<DATATYPE> RESULTS;


private:
    Node*        m_root;             ///< 루트 노드
    ELEMTYPEREAL m_unitSphereVolume; ///< 부피를 계산하기 위한 상수


public:
    CRTree();
    virtual ~CRTree();


public:
    /// \brief 데이터를 집어넣는다.
    /// \param min 바운딩 박스의 최소값
    /// \param max 바운딩 박스의 최대값
    /// \param dataId 실제 데이터의 ID. 양수여야 한다. 0은 되나 음수값은
    /// 사용할 수 없다.
    void Insert(const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS], const DATATYPE& dataId);

    /// \brief 데이터를 제거한다.
    /// \param min 바운딩 박스의 최소값
    /// \param max 바운딩 박스의 최대값
    /// \param dataId 실제 데이터의 ID. 양수여야 한다. 0은 되나 음수값은
    /// 사용할 수 없다.
    void Remove(const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS], const DATATYPE& dataId);

    /// \brief 해당하는 바운딩 박스와 겹치는 모든 데이터들을 찾는다.
    /// \param min 바운딩 박스의 최소값
    /// \param max 바운딩 박스의 최대값
    /// \param results 결과값을 집어넣을 컬렉션 객체
    /// \return 찾은 데이터의 갯수
    int Search(const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS], RESULTS& results) const;

    /// \brief 해당하는 바운딩 박스와 겹치는 모든 데이터들을 찾는다.
    /// \param min 바운딩 박스의 최소값
    /// \param max 바운딩 박스의 최대값
    /// \param results 결과값을 집어넣을 컬렉션 객체
    /// \return 찾은 데이터의 갯수
    int Contains(const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS], RESULTS& results) const;

    /// \brief 모든 데이터를 제거하고, 루트 노드를 초기화한다.
    void RemoveAll();

    /// \brief 트리 안에 있는 데이터의 갯수를 반환한다.
    ///
    /// 내부적으로 데이터의 갯수를 유지하지 않고, 이 함수가 호출될 때마다 
    /// 계산하기 때문에 느리다. 자주 호출하지 말 것.
    ///
    /// \return int 트리 안에 있는 데이터의 갯수
    int Count();


public:
    /// \brief 파일에서 트리를 로드한다.
    /// \param fileName 파일 이름
    /// \return bool 정상적으로 로드한 경우에는 true를 반환하고, 뭔가 에러가
    /// 생긴 경우에는 false를 반환한다.
    bool Load(const char* fileName);

    /// \brief 스트림에서 트리를 로드한다.
    /// \param stream 스트림 객체
    /// \return bool 정상적으로 로드한 경우에는 true를 반환하고, 뭔가 에러가
    /// 생긴 경우에는 false를 반환한다.
    bool Load(RTFileStream& stream);

    /// \brief 파일에다 트리를 저장한다.
    /// \param fileName 파일 이름
    /// \return bool 정상적으로 저장한 경우에는 true를 반환한고, 뭔가 에러가
    /// 생긴 경우에는 false를 반환한다.
    bool Save(const char* fileName);

    /// \brief 스트림에다 트리를 저장한다.
    /// \param stream 스트림 객체
    /// \return bool 정상적으로 저장한 경우에는 true를 반환한고, 뭔가 에러가
    /// 생긴 경우에는 false를 반환한다.
    bool Save(RTFileStream& stream);


public:
    //////////////////////////////////////////////////////////////////////////
    /// \class Iterator
    /// \brief 
    //////////////////////////////////////////////////////////////////////////
    class Iterator
    {
    friend CRTree;

    private:
        // Max stack size. Allows almost n^32 where n is number of branches in node
        enum { MAX_STACK = 32 }; 

        struct StackElement
        {
            Node* m_node;
            int m_branchIndex;
        };

        StackElement m_stack[MAX_STACK]; ///< 재귀호출을 피하기 위한 스택
        int          m_tos;              ///< 스택 탑

    public:
        /// \brief 생성자
        Iterator() { Init(); }

        /// \brief 소멸자
        ~Iterator() { }


    public:
        /// \brief 올바른 값을 가지고 있는가의 여부를 반환한다.
        /// \return bool 무효화된 횡단자일 경우 true
        bool IsNull() const { return m_tos <= 0; }

        /// \brief 올바른 값을 가지고 있는가의 여부를 반환한다.
        /// \return bool 유효한 횡단자일 경우 true
        bool IsNotNull() { return m_tos > 0; }

        /// \brief 현재 횡단자의 데이터 값을 반환한다. 호출하기 이전에
        /// 횡단자의 값이 정상적인지를 먼저 체크해야 한다.
        /// \return DATATYPE& 데이터 값
        DATATYPE& operator*()
        {
            assert(IsNotNull());
            StackElement& curTos = m_stack[m_tos - 1];
            return curTos.m_node->m_branch[curTos.m_branchIndex].m_data;
        } 

        /// \brief 현재 횡단자의 데이터 값을 반환한다. 호출하기 이전에
        /// 횡단자의 값이 정상적인지를 먼저 체크해야 한다.
        /// \return const DATATYPE& 데이터 값
        const DATATYPE& operator*() const
        {
            assert(IsNotNull());
            StackElement& curTos = m_stack[m_tos - 1];
            return curTos.m_node->m_branch[curTos.m_branchIndex].m_data;
        } 

        /// \brief 다음 데이터 값을 반환한다.
        /// \return bool 다음 데이터값이 존재하는 경우 true, 더 이상의 
        /// 데이터가 존재하지 않는다면 false.
        bool operator++() { return FindNextData(); }


    private:
        /// \brief 횡단자를 초기화한다.
        void Init() { m_tos = 0; }

        /// \brief 트리 안에 잇는 다음 데이터를 찾는다. (내부에서만 사용하는
        /// 함수다.)
        /// \return bool 다음 데이터값이 존재하는 경우 true, 더 이상의 
        /// 데이터가 존재하지 않는다면 false.
        bool FindNextData()
        {
            for (;;)
            {
                if (m_tos <= 0)
                    return false;

                StackElement curTos = Pop(); // 스택 탑을 복사해둔다.

                if (curTos.m_node->IsLeaf())
                {
                    // Keep walking through data while we can
                    if (curTos.m_branchIndex+1 < curTos.m_node->m_count)
                    {
                        // There is more data, just point to the next one
                        Push(curTos.m_node, curTos.m_branchIndex + 1);
                        return true;
                    }
                    // No more data, so it will fall back to previous level
                }
                else
                {
                    if (curTos.m_branchIndex+1 < curTos.m_node->m_count)
                    {
                        // Push sibling on for future tree walk
                        // This is the 'fall back' node when we finish with 
                        // the current level
                        Push(curTos.m_node, curTos.m_branchIndex + 1);
                    }

                    // Since cur node is not a leaf,
                    // push first of next level to get deeper into the tree
                    Node* nextLevelnode = curTos.m_node->m_branch[curTos.m_branchIndex].m_child;
                    Push(nextLevelnode, 0);

                    // If we pushed on a new leaf, exit as the data is ready at TOS
                    if (nextLevelnode->IsLeaf())
                        return true;
                }
            }
        }

        /// \brief 노드와 브랜치를 스택에다 푸쉬한다. (내부적으로만 사용한다.)
        /// \param node 노드
        /// \param branchIndex 브랜치 인덱스
        void Push(Node* node, int branchIndex)
        {
            m_stack[m_tos].m_node = node;
            m_stack[m_tos].m_branchIndex = branchIndex;
            ++m_tos;
            assert(m_tos <= MAX_STACK);
        }

        /// \brief 스택에서 데이터를 꺼낸다. (내부적으로만 사용한다.)
        /// \return StackElement& 스택 데이터
        StackElement& Pop()
        {
            assert(m_tos > 0);
            --m_tos;
            return m_stack[m_tos];
        }
    };

    /// \brief 제일 앞쪽의 횡단자를 리턴한다.
    /// \param it 횡단자의 참조
    void GetFirst(Iterator& it)
    {
        it.Init();
        if (m_root && (m_root->m_count > 0))
        {
            it.Push(m_root, 0);
            it.FindNextData();
        }
    }

    /// \brief 다음 횡단자를 계산한다.
    /// \param it 횡단자의 참조
    void GetNext(Iterator& it) { ++it; }

    /// \brief 횡단자가 정상적인지의 여부를 반환한다.
    /// \param it 횡단자의 참조
    bool IsNull(Iterator& it) { return it.IsNull(); }

    /// \brief 해당 횡단자의 데이터 값을 반환한다.
    /// \param it 횡단자의 참조
    DATATYPE& GetAt(Iterator& it) { return *it; }


protected:
    /// 바운딩 박스 (N 차원)
    struct Rect
    {
        ELEMTYPE m_min[NUMDIMS]; ///< 바운딩 박스의 최소값
        ELEMTYPE m_max[NUMDIMS]; ///< 바운딩 박스의 최대값
    };

    /// 실제 유저 데이터일 수도 있고, 자식 트리일 수도 있다.
    /// 부모의 레벨이 이를 결정한다. 부모의 레벨이 0이라면, 유저 데이터이다.
    struct Branch
    {
        Rect m_rect; ///< 바운딩 박스
        union
        {
            Node*    m_child; ///< 자식 노드
            DATATYPE m_data;  ///< 유저 데이터
        };
    };

    /// 실제 노드
    struct Node
    {
        /// \brief 현재 노드가 리프 노드가 아닌지의 여부를 반환한다.
        bool IsInternalNode() const { return (m_level > 0); }

        /// \brief 현재 노드가 리프 노드, 즉 실제 데이터를 가지는 노드인지의
        /// 여부를 반환한다.
        bool IsLeaf() const { return (m_level == 0); }

        int    m_count;            ///< 카운트
        int    m_level;            ///< 리프 노드일 경우 0, 다른 경우에는 양수
        Branch m_branch[MAXNODES]; ///< 자식 노드들
    };

    /// 노드 삭제 후에 새로운 삽입을 위한 링크드 리스트
    struct ListNode
    {
        ListNode* m_next; ///< Next in list
        Node*     m_node; ///< Node
    };

    /// 트리의 분할을 찾을 때 쓰이는 변수 객체
    struct PartitionVars
    {
        int          m_partition[MAXNODES+1];
        int          m_total;
        int          m_minFill;
        int          m_taken[MAXNODES+1];
        int          m_count[2];
        Rect         m_cover[2];
        ELEMTYPEREAL m_area[2];

        Branch       m_branchBuf[MAXNODES+1];
        int          m_branchCount;
        Rect         m_coverSplit;
        ELEMTYPEREAL m_coverSplitArea;
    }; 


protected:
    /// \brief 노드를 할당한다.
    /// \return Node* 새로 할당한 노드 객체
    Node* AllocNode();

    /// \brief 노드를 삭제한다.
    /// \param node 삭제할 노드
    void FreeNode(Node* node);

    /// \brief 노드를 초기화한다.
    /// \param node 초기화할 노드
    void InitNode(Node* node);

    /// \brief 바운딩 박스를 초기화한다.
    /// \param rect 초기화할 바운딩 박스
    void InitRect(Rect* rect);

    /// \brief 유저 데이터와 사각형을 트리에다 집어넣는다. 재귀적으로 트리를
    /// 따라 내려가며, 필요하다면 새로운 분할도 만든다.
    /// \param rect 바운딩 박스
    /// \param id 유저 데이터
    /// \param node 부모 노드
    /// \param newNode 노드가 분할된 경우, 새로운 노드의 값이 들어간다.
    /// \param level 리프 노드로부터 얼마나 올라왔는가를 나타내는 변수.
    /// 즉 리프/ 노드(데이터 노드)라면 이 값은 0이다.
    /// \return bool 노드가 분할되지 않았다면, 0을 반환한다. 노드가 
    /// 분할되었다면 true를 반환하고, newNode 포인터의 값을 새로운 
    /// 가르키도록 변경한다.
    bool InsertRectRec(Rect* rect, const DATATYPE& id, Node* node, Node** newNode, int level);

    /// \brief 유저 데이터와 사각형을 트리에다 집어넣는다.
    /// \param rect 바운딩 박스
    /// \param id 유저 데이터
    /// \param root 루트 노드
    /// \param level 리프 노드로부터 얼마나 올라왔는가를 나타내는 변수.
    /// 즉 리프/ 노드(데이터 노드)라면 이 값은 0이다.
    /// \return bool 노드가 분할되지 않았다면, 0을 반환한다. 노드가 분할되었다면 
    /// true를 반환한다.
    bool InsertRect(Rect* rect, const DATATYPE& id, Node** root, int level);

    /// \brief 해당하는 노드 안에 들어있는 사각형들을 모두 감싸는 가장 작은
    /// 바운딩 박스를 계산한다.
    /// \param node 노드
    /// \return Rect Minimal Bounding Box
    Rect NodeCover(Node* node);

    /// \brief 노드에다가 브랜치를 추가한다. 분할이 필요할 경우에는 분할도
    /// 한다.
    /// \param branch 추가할 브랜치
    /// \param node 노드
    /// \param newNode 노드가 분할된 경우, 새로운 노드의 값이 들어간다.
    /// \return bool 노드가 분할되지 않았다면 0을 반환한다. 분할되었다면 1을
    /// 반환한고, newNode의 값을 새로운 노드를 가르키도록 변경한다.
    bool AddBranch(Branch* branch, Node* node, Node** newNode);

    /// \brief 노드와 브랜치의 연결을 끊는다. 
    ///
    /// 이 함수를 호출한 다음에는 iterator의 값이 무효화되니 주의해야 한다.
    /// 
    /// \param node 노드
    /// \param index 브랜치 인덱스
    void DisconnectBranch(Node* node, int index);

    /// \brief 새로운 바운딩 박스를 집어넣었을 때, 부피가 가장 적게 늘어나는
    /// 브랜치를 찾는다.
    /// \param rect 바운딩 박스
    /// \param node 노드
    /// \return int 브랜치 인덱스
    int PickBranch(Rect* rect, Node* node);

    /// \brief 두 바운딩 박스를 모두 포함하는 바운딩 박스를 계산한다.
    /// \param rectA 바운딩 박스
    /// \param rectB 바운딩 박스
    /// \return Rect 두 바운딩 박스를 모두 포함하는 바운딩 박스
    Rect CombineRect(Rect* rectA, Rect* rectB);

    /// \brief 노드를 분할한다.
    ///
    /// 주어진 노드의 브랜치 + 새로운 브랜치 == 2 노드로 분할한다.
    ///
    /// \param node 노드 (변경된다)
    /// \param branch 새로운 브랜치
    /// \param newNode 새로 생성한 노드
    void SplitNode(Node* node, Branch* branch, Node** newNode);

    /// \brief 바운딩 박스의 구 부피 값을 반환한다.
    /// \param rect 바운딩 박스
    /// \return ELEMTYPEREAL 부피 값
    ELEMTYPEREAL RectSphericalVolume(Rect* rect);

    /// \brief 바운딩 박스의 부피를 계산한다.
    /// \param rect 바운딩 박스
    /// \return ELEMTYPEREAL 부피 값
    ELEMTYPEREAL RectVolume(Rect* rect);

    /// \brief 바운딩 박스의 부피를 계산한다.
    /// \param rect 바운딩 박스
    /// \return ELEMTYPEREAL 부피
    ELEMTYPEREAL CalcRectVolume(Rect* rect);

    /// \brief 가득찬 노드 하나 + 브랜치 하나를 읽어들여 버퍼를 채운다.
    /// \param node 노드
    /// \param branch 브랜치
    /// \param parVars 버퍼
    void GetBranches(Node* node, Branch* branch, PartitionVars* parVars);

    /// \brief 분할을 선택하기 위한 함수
    ///
    /// As the seeds for the two groups, pick the two rects that would waste the
    /// most area if covered by a single rectangle, i.e. evidently the worst pair
    /// to have in the same group.
    /// Of the remaining, one at a time is chosen to be put in one of the two 
    /// groups. The one chosen is the one with the greatest difference in area 
    /// expansion depending on which group - the rect most strongly attracted to
    /// one group and repelled from the other.
    /// If one group gets too full (more would force other group to violate min
    /// fill requirement) then other group gets the rest.
    /// These last are the ones that can go in either group most easily.
    /// 
    /// \param parVars 버퍼
    /// \param minFill 최소 채우기 값
    void ChoosePartition(PartitionVars* parVars, int minFill);

    /// \brief 분할에 따라, 브랜드치들을 버퍼에서 복사한다.
    /// \param nodeA 노드
    /// \param nodeB 노드
    /// \param parVars 버퍼
    void LoadNodes(Node* nodeA, Node* nodeB, PartitionVars* parVars);

    /// \brief 트리의 분할을 찾을 때 쓰이는 변수 객체를 초기화한다.
    /// \param parVars 변수 객체
    /// \param maxRects 사격형의 갯수
    /// \param minFill 최소 채우기 값
    void InitParVars(PartitionVars* parVars, int maxRects, int minFill);

    /// \brief 트리의 분할을 위한 시드 값을 계산한다.
    /// \param parVars 트리의 분할을 찾을 때 쓰이는 변수 객체
    void PickSeeds(PartitionVars* parVars);

    /// \brief 브랜치를 그룹별로 구분한다.
    /// \param index 브랜치의 인덱스
    /// \param group 그룹 인덱스
    /// \param parVars 트리의 분할을 찾을 때 쓰이는 변수 객체
    void Classify(int index, int group, PartitionVars* parVars);

    /// \brief 바운딩 박스를 삭제한다.
    /// \param rect 제거할 바운딩 박스
    /// \param id 제거할 유저 데이터
    /// \param root 루트 노드
    /// \return bool 성공적으로 삭제한 경우에는 0을 반환하고, 해당 데이터를 찾지
    /// 못한 경우에는 1을 반환한다.
    bool RemoveRect(Rect* rect, const DATATYPE& id, Node** root);

    /// \brief 루트가 아닌 다른 노드에서 바운딩 박스를 삭제한다.
    ///
    /// RemoveRect 함수에서 호출한다. 트리를 따라내려가며, 노드를 합쳐야하는
    /// 경우에는 합친다.
    ///
    /// \param rect 제거할 바운딩 박스
    /// \param id 제거할 유저 데이터
    /// \param node 부모 노드
    /// \param listNode 재삽입 리스트 객체
    /// \return bool 성공적으로 삭제한 경우에는 0을 반환하고, 해당 데이터를 찾지
    /// 못한 경우에는 1을 반환한다.
    bool RemoveRectRec(Rect* rect, const DATATYPE& id, Node* node, ListNode** listNode);

    /// \brief 리스트 노드를 할당한다.
    /// \return ListNode* 새로 할당한 리스트 노드
    ListNode* AllocListNode();

    /// \brief 리스트 노드를 삭제한다.
    /// \param listNode 삭제할 리스트 노드
    void FreeListNode(ListNode* listNode);

    /// \brief 두 바운딩 박스가 겹치는지의 여부를 반환한다.
    /// \param rectA 바운딩 박스
    /// \param rectB 바운딩 박스
    /// \return bool 겹치는 경우 true, 겹치지 않는 경우 false
    bool Overlap(Rect* rectA, Rect* rectB) const;

	/// \brief 두 바운딩 박스가 겹치는지의 여부를 반환한다.
    /// \param rectA 바운딩 박스
    /// \param rectB 바운딩 박스
    /// \return bool 겹치는 경우 true, 겹치지 않는 경우 false
    bool Contain(Rect* rectA, Rect* rectB) const;

    /// \brief 노드를 재삽입 리스트에다 추가한다. 해당 노드와 브랜치들은 
    /// 나중에 트리에 새로이 추가한다.
    /// \param node 노드
    /// \param listNode 리스트 객체
    void ReInsert(Node* node, ListNode** listNode);

    /// \brief 실제 검색 함수
    /// \param node 검색할 노드
    /// \param rect 바운딩 박스
    /// \param foundCount 찾은 데이터의 갯수
    /// \param results 찾은 데이터를 집어넣을 컬렉션
    /// \return bool 상위 함수에서 검색을 계속해야하는 경우에는 true, 검색을
    /// 중단해야하는 경우에는 false.
    bool Search(Node* node, Rect* rect, int& foundCount, RESULTS& results) const;

	/// \brief 실제 검색 함수
    /// \param node 검색할 노드
    /// \param rect 바운딩 박스
    /// \param foundCount 찾은 데이터의 갯수
    /// \param results 찾은 데이터를 집어넣을 컬렉션
    /// \return bool 상위 함수에서 검색을 계속해야하는 경우에는 true, 검색을
    /// 중단해야하는 경우에는 false.
    bool Contains(Node* node, Rect* rect, int& foundCount, RESULTS& results) const;

    /// \brief 해당하는 노드와 자식 노드들을 모두 삭제한다.
    /// \param node 삭제할 노드
    void RemoveAllRec(Node* node);

    /// \brief 모든 노드들을 삭제한다.
    void Reset();

    /// \brief 트리 안에 있는 데이터의 갯수를 센다.
    /// \param node 부모 노드의 포인터
    /// \param count 데이터의 갯수를 집어넣을 변수
    void CountRec(Node* node, int& count);

    /// \brief 스트림에서 레코드를 읽어들인다.
    /// \param node 부모 노드
    /// \param stream 레코드를 읽어들일 스트림 객체
    /// \return bool 정상적으로 로드한 경우에는 true를 반환하고, 뭔가 에러가
    /// 생긴 경우에는 false를 반환한다.
    bool LoadRec(Node* node, RTFileStream& stream);

    /// \brief 스트림에다 레코드를 저장한다.
    /// \param node 부모 노드
    /// \param stream 레코드를 저장할 스트림 객체
    /// \return bool 정상적으로 저장한 경우에는 true를 반환한고, 뭔가 에러가
    /// 생긴 경우에는 false를 반환한다.
    bool SaveRec(Node* node, RTFileStream& stream);
};


//////////////////////////////////////////////////////////////////////////////
/// \class RTFileStream
/// \brief Because there is not stream support, this is a quick and dirty 
/// file I/O helper. Users will likely replace its usage with a Stream 
/// implementation from their favorite API.
//////////////////////////////////////////////////////////////////////////////

class RTFileStream
{
    FILE* m_file;

public:
    RTFileStream() : m_file(NULL) {}
    ~RTFileStream() { Close(); }

    bool OpenRead(const char* fileName)
    {
        m_file = fopen(fileName, "rb");
        return !m_file ? false : true;
    }

    bool OpenWrite(const char* fileName)
    {
        m_file = fopen(fileName, "wb");
        return !m_file ? false : true;
    }

    void Close()
    {
        if (m_file)
        {
            fclose(m_file);
            m_file = NULL;
        }
    }

    template< class TYPE >
    size_t Write(const TYPE& value)
    {
        assert(m_file);
        return fwrite((void*)&value, sizeof(value), 1, m_file);
    }

    template< class TYPE >
    size_t WriteArray(const TYPE* array, int count)
    {
        assert(m_file);
        return fwrite((void*)array, sizeof(TYPE) * count, 1, m_file);
    }

    template< class TYPE >
    size_t Read(TYPE& value)
    {
        assert(m_file);
        return fread((void*)&value, sizeof(value), 1, m_file);
    }

    template< class TYPE >
    size_t ReadArray(TYPE* array, int count)
    {
        assert(m_file);
        return fread((void*)array, sizeof(TYPE) * count, 1, m_file);
    }
};


#define RTREE_TEMPLATE template<class DATATYPE, class ELEMTYPE, int NUMDIMS, class ELEMTYPEREAL, int TMAXNODES, int TMINNODES>
#define RTREE_QUALIFIER CRTree<DATATYPE, ELEMTYPE, NUMDIMS, ELEMTYPEREAL, TMAXNODES, TMINNODES>


//////////////////////////////////////////////////////////////////////////////
/// \brief 생성자
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
RTREE_QUALIFIER::CRTree()
{
    assert(MAXNODES > MINNODES);
    assert(MINNODES > 0);

    // 위에서도 설명했듯이, int 크기까지의 데이터만 다룰 수 있다.
    // 자세한 것은 Branch 구조체를 보면 알 것이다.
    assert(sizeof(DATATYPE) == sizeof(void*) || sizeof(DATATYPE) == sizeof(int));

    // Precomputed volumes of the unit spheres for the first few dimensions
    const float UNIT_SPHERE_VOLUMES[] = 
    {
        0.000000f, 2.000000f, 3.141593f, // Dimension  0,1,2
        4.188790f, 4.934802f, 5.263789f, // Dimension  3,4,5
        5.167713f, 4.724766f, 4.058712f, // Dimension  6,7,8
        3.298509f, 2.550164f, 1.884104f, // Dimension  9,10,11
        1.335263f, 0.910629f, 0.599265f, // Dimension  12,13,14
        0.381443f, 0.235331f, 0.140981f, // Dimension  15,16,17
        0.082146f, 0.046622f, 0.025807f  // Dimension  18,19,20 
    };

    m_root             = AllocNode();
    m_root->m_level    = 0;
    m_unitSphereVolume = (ELEMTYPEREAL)UNIT_SPHERE_VOLUMES[NUMDIMS];
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 소멸자
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
RTREE_QUALIFIER::~CRTree()
{
    Reset(); // Free, or reset node memory
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 데이터를 집어넣는다.
/// \param min 바운딩 박스의 최소값
/// \param max 바운딩 박스의 최대값
/// \param dataId 실제 데이터의 ID. 양수여야 한다. 0은 되나 음수값은
/// 사용할 수 없다.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::Insert(const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS], const DATATYPE& dataId)
{
#ifdef _DEBUG
    for (int index=0; index<NUMDIMS; ++index)
    {
        assert(min[index] <= max[index]);
    }
#endif //_DEBUG

    Rect rect;

    for (int axis=0; axis<NUMDIMS; ++axis)
    {
        rect.m_min[axis] = min[axis];
        rect.m_max[axis] = max[axis];
    }

    InsertRect(&rect, dataId, &m_root, 0);
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 데이터를 제거한다.
/// \param min 바운딩 박스의 최소값
/// \param max 바운딩 박스의 최대값
/// \param dataId 실제 데이터의 ID. 양수여야 한다. 0은 되나 음수값은
/// 사용할 수 없다.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::Remove(const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS], const DATATYPE& dataId)
{
#ifdef _DEBUG
    for (int index=0; index<NUMDIMS; ++index)
    {
        assert(min[index] <= max[index]);
    }
#endif //_DEBUG

    Rect rect;

    for (int axis=0; axis<NUMDIMS; ++axis)
    {
        rect.m_min[axis] = min[axis];
        rect.m_max[axis] = max[axis];
    }

    RemoveRect(&rect, dataId, &m_root);
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 해당하는 바운딩 박스에 포함되는 모든 데이터들을 찾는다.
/// \param min 바운딩 박스의 최소값
/// \param max 바운딩 박스의 최대값
/// \param results 결과값을 집어넣을 컬렉션 객체
/// \return 찾은 데이터의 갯수
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
int RTREE_QUALIFIER::Search(const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS], RESULTS& results) const
{
#ifdef _DEBUG
    for (int index=0; index<NUMDIMS; ++index)
    {
        assert(min[index] <= max[index]);
    }
#endif //_DEBUG

    Rect rect;
    for (int axis=0; axis<NUMDIMS; ++axis)
    {
        rect.m_min[axis] = min[axis];
        rect.m_max[axis] = max[axis];
    }

    // NOTE: May want to return search result another way,
    // perhaps returning the number of found elements here.
    int foundCount = 0;
    Search(m_root, &rect, foundCount, results);
    return foundCount;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief 해당하는 바운딩 박스에 포함되는 모든 데이터들을 찾는다.
/// \param min 바운딩 박스의 최소값
/// \param max 바운딩 박스의 최대값
/// \param results 결과값을 집어넣을 컬렉션 객체
/// \return 찾은 데이터의 갯수
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
int RTREE_QUALIFIER::Contains(const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS], RESULTS& results) const
{
#ifdef _DEBUG
    for (int index=0; index<NUMDIMS; ++index)
    {
        assert(min[index] <= max[index]);
    }
#endif //_DEBUG

    Rect rect;
    for (int axis=0; axis<NUMDIMS; ++axis)
    {
        rect.m_min[axis] = min[axis];
        rect.m_max[axis] = max[axis];
    }

    // NOTE: May want to return search result another way,
    // perhaps returning the number of found elements here.
    int foundCount = 0;
    Contains(m_root, &rect, foundCount, results);

    return foundCount;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief 트리 안에 있는 데이터의 갯수를 반환한다.
///
/// 내부적으로 데이터의 갯수를 유지하지 않고, 이 함수가 호출될 때마다 
/// 계산하기 때문에 느리다. 자주 호출하지 말 것.
///
/// \return int 트리 안에 있는 데이터의 갯수
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
int RTREE_QUALIFIER::Count()
{
    int count = 0;
    CountRec(m_root, count);
    return count;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 트리 안에 있는 데이터의 갯수를 센다.
/// \param node 부모 노드의 포인터
/// \param count 데이터의 갯수를 집어넣을 변수
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::CountRec(Node* node, int& count)
{
    if (node->IsInternalNode())  // not a leaf node
    {
        for (int index = 0; index < node->m_count; ++index)
        {
            CountRec(node->m_branch[index].m_child, count);
        }
    }
    else // A leaf node
    {
        count += node->m_count;
    }
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 파일에서 트리를 로드한다.
/// \param fileName 파일 이름
/// \return bool 정상적으로 로드한 경우에는 true를 반환하고, 뭔가 에러가
/// 생긴 경우에는 false를 반환한다.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
bool RTREE_QUALIFIER::Load(const char* fileName)
{
    RemoveAll(); // Clear existing tree

    RTFileStream stream;
    if (!stream.OpenRead(fileName))
    {
        return false;
    }

    bool result = Load(stream);

    stream.Close();

    return result;
};


//////////////////////////////////////////////////////////////////////////////
/// \brief 스트림에서 트리를 로드한다.
/// \param stream 스트림 객체
/// \return bool 정상적으로 로드한 경우에는 true를 반환하고, 뭔가 에러가
/// 생긴 경우에는 false를 반환한다.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
bool RTREE_QUALIFIER::Load(RTFileStream& stream)
{
    // Write some kind of header
    int _dataFileId = ('R'<<0)|('T'<<8)|('R'<<16)|('E'<<24);
    int _dataSize = sizeof(DATATYPE);
    int _dataNumDims = NUMDIMS;
    int _dataElemSize = sizeof(ELEMTYPE);
    int _dataElemRealSize = sizeof(ELEMTYPEREAL);
    int _dataMaxNodes = TMAXNODES;
    int _dataMinNodes = TMINNODES;

    int dataFileId = 0;
    int dataSize = 0;
    int dataNumDims = 0;
    int dataElemSize = 0;
    int dataElemRealSize = 0;
    int dataMaxNodes = 0;
    int dataMinNodes = 0;

    stream.Read(dataFileId);
    stream.Read(dataSize);
    stream.Read(dataNumDims);
    stream.Read(dataElemSize);
    stream.Read(dataElemRealSize);
    stream.Read(dataMaxNodes);
    stream.Read(dataMinNodes);

    bool result = false;

    // Test if header was valid and compatible
    if (   (dataFileId == _dataFileId) 
        && (dataSize == _dataSize) 
        && (dataNumDims == _dataNumDims) 
        && (dataElemSize == _dataElemSize) 
        && (dataElemRealSize == _dataElemRealSize) 
        && (dataMaxNodes == _dataMaxNodes) 
        && (dataMinNodes == _dataMinNodes) 
        )
    {
        // Recursively load tree
        result = LoadRec(m_root, stream);
    }

    return result;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 스트림에서 레코드를 읽어들인다.
/// \param node 부모 노드
/// \param stream 레코드를 읽어들일 스트림 객체
/// \return bool 정상적으로 로드한 경우에는 true를 반환하고, 뭔가 에러가
/// 생긴 경우에는 false를 반환한다.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
bool RTREE_QUALIFIER::LoadRec(Node* node, RTFileStream& stream)
{
    stream.Read(node->m_level);
    stream.Read(node->m_count);

    if (node->IsInternalNode())  // not a leaf node
    {
        for (int index = 0; index < node->m_count; ++index)
        {
            Branch* curBranch = &node->m_branch[index];

            stream.ReadArray(curBranch->m_rect.m_min, NUMDIMS);
            stream.ReadArray(curBranch->m_rect.m_max, NUMDIMS);

            curBranch->m_child = AllocNode();
            LoadRec(curBranch->m_child, stream);
        }
    }
    else // A leaf node
    {
        for (int index = 0; index < node->m_count; ++index)
        {
            Branch* curBranch = &node->m_branch[index];

            stream.ReadArray(curBranch->m_rect.m_min, NUMDIMS);
            stream.ReadArray(curBranch->m_rect.m_max, NUMDIMS);

            stream.Read(curBranch->m_data);
        }
    }

    return true; // Should do more error checking on I/O operations
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 파일에다 트리를 저장한다.
/// \param fileName 파일 이름
/// \return bool 정상적으로 저장한 경우에는 true를 반환한고, 뭔가 에러가
/// 생긴 경우에는 false를 반환한다.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
bool RTREE_QUALIFIER::Save(const char* fileName)
{
    RTFileStream stream;
    if (!stream.OpenWrite(fileName))
    {
        return false;
    }

    bool result = Save(stream);

    stream.Close();

    return result;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 스트림에다 트리를 저장한다.
/// \param stream 스트림 객체
/// \return bool 정상적으로 저장한 경우에는 true를 반환한고, 뭔가 에러가
/// 생긴 경우에는 false를 반환한다.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
bool RTREE_QUALIFIER::Save(RTFileStream& stream)
{
    // Write some kind of header
    int dataFileId = ('R'<<0)|('T'<<8)|('R'<<16)|('E'<<24);
    int dataSize = sizeof(DATATYPE);
    int dataNumDims = NUMDIMS;
    int dataElemSize = sizeof(ELEMTYPE);
    int dataElemRealSize = sizeof(ELEMTYPEREAL);
    int dataMaxNodes = TMAXNODES;
    int dataMinNodes = TMINNODES;

    stream.Write(dataFileId);
    stream.Write(dataSize);
    stream.Write(dataNumDims);
    stream.Write(dataElemSize);
    stream.Write(dataElemRealSize);
    stream.Write(dataMaxNodes);
    stream.Write(dataMinNodes);

    // Recursively save tree
    bool result = SaveRec(m_root, stream);

    return result;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 스트림에다 레코드를 저장한다.
/// \param node 부모 노드
/// \param stream 레코드를 저장할 스트림 객체
/// \return bool 정상적으로 저장한 경우에는 true를 반환한고, 뭔가 에러가
/// 생긴 경우에는 false를 반환한다.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
bool RTREE_QUALIFIER::SaveRec(Node* node, RTFileStream& stream)
{
    stream.Write(node->m_level);
    stream.Write(node->m_count);

    if (node->IsInternalNode())  // not a leaf node
    {
        for (int index = 0; index < node->m_count; ++index)
        {
            Branch* curBranch = &node->m_branch[index];

            stream.WriteArray(curBranch->m_rect.m_min, NUMDIMS);
            stream.WriteArray(curBranch->m_rect.m_max, NUMDIMS);

            SaveRec(curBranch->m_child, stream);
        }
    }
    else // A leaf node
    {
        for (int index = 0; index < node->m_count; ++index)
        {
            Branch* curBranch = &node->m_branch[index];

            stream.WriteArray(curBranch->m_rect.m_min, NUMDIMS);
            stream.WriteArray(curBranch->m_rect.m_max, NUMDIMS);

            stream.Write(curBranch->m_data);
        }
    }

    return true; // Should do more error checking on I/O operations
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 모든 데이터를 제거하고, 루트 노드를 초기화한다.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::RemoveAll()
{
    Reset();

    m_root = AllocNode();
    m_root->m_level = 0;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 모든 노드들을 삭제한다.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::Reset()
{
#ifdef RTREE_DONT_USE_MEMPOOLS
    // Delete all existing nodes
    RemoveAllRec(m_root);
#else // RTREE_DONT_USE_MEMPOOLS
    // Just reset memory pools.  We are not using complex types
    // EXAMPLE
#endif // RTREE_DONT_USE_MEMPOOLS
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 해당하는 노드와 자식 노드들을 모두 삭제한다.
/// \param node 삭제할 노드
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::RemoveAllRec(Node* node)
{
    assert(node);
    assert(node->m_level >= 0);

    if (node->IsInternalNode()) // This is an internal node in the tree
    {
        for (int index=0; index < node->m_count; ++index)
        {
            RemoveAllRec(node->m_branch[index].m_child);
        }
    }

    FreeNode(node); 
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 노드를 할당한다.
/// \return Node* 새로 할당한 노드 객체
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
typename RTREE_QUALIFIER::Node* RTREE_QUALIFIER::AllocNode()
{
    Node* newNode;
#ifdef RTREE_DONT_USE_MEMPOOLS
    newNode = new Node;
#else // RTREE_DONT_USE_MEMPOOLS
    // EXAMPLE
#endif // RTREE_DONT_USE_MEMPOOLS
    InitNode(newNode);
    return newNode;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 노드를 삭제한다.
/// \param node 삭제할 노드
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::FreeNode(Node* node)
{
    assert(node);

#ifdef RTREE_DONT_USE_MEMPOOLS
    delete node;
#else // RTREE_DONT_USE_MEMPOOLS
    // EXAMPLE
#endif // RTREE_DONT_USE_MEMPOOLS
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 리스트 노드를 할당한다.
/// \return ListNode* 새로 할당한 리스트 노드
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
typename RTREE_QUALIFIER::ListNode* RTREE_QUALIFIER::AllocListNode()
{
#ifdef RTREE_DONT_USE_MEMPOOLS
    return new ListNode;
#else // RTREE_DONT_USE_MEMPOOLS
    // EXAMPLE
#endif // RTREE_DONT_USE_MEMPOOLS
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 리스트 노드를 삭제한다.
/// \param listNode 삭제할 리스트 노드
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::FreeListNode(ListNode* listNode)
{
#ifdef RTREE_DONT_USE_MEMPOOLS
    delete listNode;
#else // RTREE_DONT_USE_MEMPOOLS
    // EXAMPLE
#endif // RTREE_DONT_USE_MEMPOOLS
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 노드를 초기화한다.
/// \param node 초기화할 노드
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::InitNode(Node* node)
{
    node->m_count = 0;
    node->m_level = -1;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 바운딩 박스를 초기화한다.
/// \param rect 초기화할 바운딩 박스
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::InitRect(Rect* rect)
{
    for (int index = 0; index < NUMDIMS; ++index)
    {
        rect->m_min[index] = (ELEMTYPE)0;
        rect->m_max[index] = (ELEMTYPE)0;
    }
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 유저 데이터와 사각형을 트리에다 집어넣는다. 재귀적으로 트리를
/// 따라 내려가며, 필요하다면 새로운 분할도 만든다.
/// \param rect 바운딩 박스
/// \param id 유저 데이터
/// \param node 부모 노드
/// \param newNode 노드가 분할된 경우, 새로운 노드의 값이 들어간다.
/// \param level 리프 노드로부터 얼마나 올라왔는가를 나타내는 변수. 
/// 즉 리프 노드(데이터 노드)라면 이 값은 0이다.
/// \return bool 노드가 분할되지 않았다면, 0을 반환한다. 노드가 분할되었다면
/// true를 반환하고, newNode 포인터의 값을 새로운 가르키도록 변경한다.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
bool RTREE_QUALIFIER::InsertRectRec(Rect* rect, const DATATYPE& id, Node* node, Node** newNode, int level)
{
    assert(rect && node && newNode);
    assert(level >= 0 && level <= node->m_level);

    int index;
    Branch branch;
    Node* otherNode;

    // Still above level for insertion, go down tree recursively
    if (node->m_level > level)
    {
        index = PickBranch(rect, node);
        if (!InsertRectRec(rect, id, node->m_branch[index].m_child, &otherNode, level))
        {
            // Child was not split
            node->m_branch[index].m_rect = CombineRect(rect, &(node->m_branch[index].m_rect));
            return false;
        }
        else // Child was split
        {
            node->m_branch[index].m_rect = NodeCover(node->m_branch[index].m_child);
            branch.m_child = otherNode;
            branch.m_rect = NodeCover(otherNode);
            return AddBranch(&branch, node, newNode);
        }
    }
    else if (node->m_level == level) // Have reached level for insertion. Add rect, split if necessary
    {
        branch.m_rect = *rect;
        branch.m_child = (Node*)id;
        // Child field of leaves contains id of data record
        return AddBranch(&branch, node, newNode);
    }
    else
    {
        // Should never occur
        assert(0);
        return false;
    }
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 유저 데이터와 사각형을 트리에다 집어넣는다.
/// 
/// 실제로 트리의 분할을 담당하는 함수는 InsertRectRec 함수다.
///
/// \param rect 바운딩 박스
/// \param id 유저 데이터
/// \param root 루트 노드
/// \param level 리프 노드로부터 얼마나 올라왔는가를 나타내는 변수.
/// 즉 리프/ 노드(데이터 노드)라면 이 값은 0이다.
/// \return bool 노드가 분할되지 않았다면, 0을 반환한다. 노드가 분할되었다면 
/// true를 반환한다.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
bool RTREE_QUALIFIER::InsertRect(Rect* rect, const DATATYPE& id, Node** root, int level)
{
    assert(rect && root);
    assert(level >= 0 && level <= (*root)->m_level);
#ifdef _DEBUG
    for (int index=0; index < NUMDIMS; ++index)
    {
        assert(rect->m_min[index] <= rect->m_max[index]);
    }
#endif //_DEBUG  

    Node* newRoot = NULL;
    Node* newNode = NULL;
    Branch branch;

    if (InsertRectRec(rect, id, *root, &newNode, level))  // Root split
    {
        newRoot = AllocNode();  // Grow tree taller and new root
        newRoot->m_level = (*root)->m_level + 1;
        branch.m_rect = NodeCover(*root);
        branch.m_child = *root;
        AddBranch(&branch, newRoot, NULL);
        branch.m_rect = NodeCover(newNode);
        branch.m_child = newNode;
        AddBranch(&branch, newRoot, NULL);
        *root = newRoot;
        return true;
    }

    return false;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 해당하는 노드 안에 들어있는 사각형들을 모두 감싸는 가장 작은
/// 바운딩 박스를 계산한다.
/// \param node 노드
/// \return Rect Minimal Bounding Box
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
typename RTREE_QUALIFIER::Rect RTREE_QUALIFIER::NodeCover(Node* node)
{
    assert(node);

    int firstTime = true;
    Rect rect;
    InitRect(&rect);

    for (int index = 0; index < node->m_count; ++index)
    {
        if (firstTime)
        {
            rect = node->m_branch[index].m_rect;
            firstTime = false;
        }
        else
        {
            rect = CombineRect(&rect, &(node->m_branch[index].m_rect));
        }
    }

    return rect;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 노드에다가 브랜치를 추가한다. 분할이 필요할 경우에는 분할도
/// 한다.
/// \param branch 추가할 브랜치
/// \param node 노드
/// \param newNode 노드가 분할된 경우, 새로운 노드의 값이 들어간다.
/// \return bool 노드가 분할되지 않았다면 0을 반환한다. 분할되었다면 1을
/// 반환한고, newNode의 값을 새로운 노드를 가르키도록 변경한다.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
bool RTREE_QUALIFIER::AddBranch(Branch* branch, Node* node, Node** newNode)
{
    assert(branch);
    assert(node);

    if (node->m_count < MAXNODES)  // Split won't be necessary
    {
        node->m_branch[node->m_count] = *branch;
        ++node->m_count;

        return false;
    }
    else
    {
        assert(newNode);

        SplitNode(node, branch, newNode);
        return true;
    }
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 노드와 브랜치의 연결을 끊는다. 
///
/// 이 함수를 호출한 다음에는 iterator의 값이 무효화되니 주의해야 한다.
/// 
/// \param node 노드
/// \param index 브랜치 인덱스
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::DisconnectBranch(Node* node, int index)
{
    assert(node && (index >= 0) && (index < MAXNODES));
    assert(node->m_count > 0);

    // Remove element by swapping with the last element to prevent gaps in array
    node->m_branch[index] = node->m_branch[node->m_count - 1];

    --node->m_count;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 새로운 바운딩 박스를 집어넣었을 때, 부피가 가장 적게 늘어나는
/// 브랜치를 찾는다.
/// \param rect 바운딩 박스
/// \param node 노드
/// \return int 브랜치 인덱스
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
int RTREE_QUALIFIER::PickBranch(Rect* rect, Node* node)
{
    assert(rect && node);

    bool         firstTime = true;
    ELEMTYPEREAL increase = 0;
    ELEMTYPEREAL bestIncr = (ELEMTYPEREAL)-1;
    ELEMTYPEREAL area = 0;
    ELEMTYPEREAL bestArea = 0;
    int          best = 0;
    Rect         tempRect;

    for (int index=0; index < node->m_count; ++index)
    {
        Rect* curRect = &node->m_branch[index].m_rect;
        area = CalcRectVolume(curRect);
        tempRect = CombineRect(rect, curRect);
        increase = CalcRectVolume(&tempRect) - area;
        if ((increase < bestIncr) || firstTime)
        {
            best = index;
            bestArea = area;
            bestIncr = increase;
            firstTime = false;
        }
        else if ((increase == bestIncr) && (area < bestArea))
        {
            best = index;
            bestArea = area;
            bestIncr = increase;
        }
    }

    return best;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 두 바운딩 박스를 모두 포함하는 바운딩 박스를 계산한다.
/// \param rectA 바운딩 박스
/// \param rectB 바운딩 박스
/// \return Rect 두 바운딩 박스를 모두 포함하는 바운딩 박스
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
typename RTREE_QUALIFIER::Rect RTREE_QUALIFIER::CombineRect(Rect* rectA, Rect* rectB)
{
    assert(rectA && rectB);

    Rect newRect;

    for (int index = 0; index < NUMDIMS; ++index)
    {
        newRect.m_min[index] = __min(rectA->m_min[index], rectB->m_min[index]);
        newRect.m_max[index] = __max(rectA->m_max[index], rectB->m_max[index]);
    }

    return newRect;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 노드를 분할한다.
///
/// 주어진 노드의 브랜치 + 새로운 브랜치 == 2 노드로 분할한다.
///
/// \param node 노드 (변경된다)
/// \param branch 새로운 브랜치
/// \param newNode 새로 생성한 노드
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::SplitNode(Node* node, Branch* branch, Node** newNode)
{
    assert(node);
    assert(branch);

    // Could just use local here, but member or external is faster since it is reused
    PartitionVars localVars;
    PartitionVars* parVars = &localVars;
    int level;

    // Load all the branches into a buffer, initialize old node
    level = node->m_level;
    GetBranches(node, branch, parVars);

    // Find partition
    ChoosePartition(parVars, MINNODES);

    // Put branches from buffer into 2 nodes according to chosen partition
    *newNode = AllocNode();
    (*newNode)->m_level = node->m_level = level;
    LoadNodes(node, *newNode, parVars);

    assert((node->m_count + (*newNode)->m_count) == parVars->m_total);
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 바운딩 박스의 부피를 계산한다.
/// \param rect 바운딩 박스
/// \return ELEMTYPEREAL 부피 값
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
ELEMTYPEREAL RTREE_QUALIFIER::RectVolume(Rect* rect)
{
    assert(rect);

    ELEMTYPEREAL volume = (ELEMTYPEREAL)1;

    for (int index=0; index<NUMDIMS; ++index)
    {
        volume *= rect->m_max[index] - rect->m_min[index];
    }

    assert(volume >= (ELEMTYPEREAL)0);

    return volume;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 바운딩 박스의 구 부피 값을 반환한다.
/// \param rect 바운딩 박스
/// \return ELEMTYPEREAL 부피 값
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
ELEMTYPEREAL RTREE_QUALIFIER::RectSphericalVolume(Rect* rect)
{
    assert(rect);

    ELEMTYPEREAL sumOfSquares = (ELEMTYPEREAL)0;
    ELEMTYPEREAL radius;

    for (int index=0; index < NUMDIMS; ++index) 
    {
        ELEMTYPEREAL halfExtent = ((ELEMTYPEREAL)rect->m_max[index] - (ELEMTYPEREAL)rect->m_min[index]) * 0.5f;
        sumOfSquares += halfExtent * halfExtent;
    }

    radius = (ELEMTYPEREAL)sqrt(sumOfSquares);

    // Pow maybe slow, so test for common dims like 2,3 and just use x*x, x*x*x.
    if (NUMDIMS == 3)
    {
        return (radius * radius * radius * m_unitSphereVolume);
    }
    else if (NUMDIMS == 2)
    {
        return (radius * radius * m_unitSphereVolume);
    }
    else
    {
        return (ELEMTYPEREAL)(pow(radius, NUMDIMS) * m_unitSphereVolume);
    }
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 바운딩 박스의 부피를 계산한다.
/// \param rect 바운딩 박스
/// \return ELEMTYPEREAL 부피
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
ELEMTYPEREAL RTREE_QUALIFIER::CalcRectVolume(Rect* rect)
{
#ifdef RTREE_USE_SPHERICAL_VOLUME
    return RectSphericalVolume(rect); // Slower but helps certain merge cases
#else // RTREE_USE_SPHERICAL_VOLUME
    return RectVolume(rect); // Faster but can cause poor merges
#endif // RTREE_USE_SPHERICAL_VOLUME  
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 가득찬 노드 하나 + 브랜치 하나를 읽어들여 버퍼를 채운다.
/// \param node 노드
/// \param branch 브랜치
/// \param parVars 버퍼
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::GetBranches(Node* node, Branch* branch, PartitionVars* parVars)
{
    assert(node);
    assert(branch);
    assert(node->m_count == MAXNODES);

    // Load the branch buffer
    for (int index=0; index < MAXNODES; ++index)
    {
        parVars->m_branchBuf[index] = node->m_branch[index];
    }
    parVars->m_branchBuf[MAXNODES] = *branch;
    parVars->m_branchCount = MAXNODES + 1;

    // Calculate rect containing all in the set
    parVars->m_coverSplit = parVars->m_branchBuf[0].m_rect;
    for (int index_=1; index_ < MAXNODES+1; ++index_)
    {
        parVars->m_coverSplit = CombineRect(
            &parVars->m_coverSplit, &parVars->m_branchBuf[index_].m_rect);
    }
    parVars->m_coverSplitArea = CalcRectVolume(&parVars->m_coverSplit);

    InitNode(node);
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 분할을 선택하기 위한 함수
///
/// As the seeds for the two groups, pick the two rects that would waste the
/// most area if covered by a single rectangle, i.e. evidently the worst pair
/// to have in the same group.
/// Of the remaining, one at a time is chosen to be put in one of the two 
/// groups. The one chosen is the one with the greatest difference in area 
/// expansion depending on which group - the rect most strongly attracted to
/// one group and repelled from the other.
/// If one group gets too full (more would force other group to violate min
/// fill requirement) then other group gets the rest.
/// These last are the ones that can go in either group most easily.
/// 
/// \param parVars 버퍼
/// \param minFill 최소 채우기 값
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::ChoosePartition(PartitionVars* parVars, int minFill)
{
    assert(parVars);

    ELEMTYPEREAL biggestDiff;
    int group, chosen, betterGroup;

    InitParVars(parVars, parVars->m_branchCount, minFill);
    PickSeeds(parVars);

    while (((parVars->m_count[0] + parVars->m_count[1]) < parVars->m_total)
        && (parVars->m_count[0] < (parVars->m_total - parVars->m_minFill))
        && (parVars->m_count[1] < (parVars->m_total - parVars->m_minFill)))
    {
        biggestDiff = (ELEMTYPEREAL) -1;
        for (int index=0; index<parVars->m_total; ++index)
        {
            if (!parVars->m_taken[index])
            {
                Rect* curRect = &parVars->m_branchBuf[index].m_rect;
                Rect rect0 = CombineRect(curRect, &parVars->m_cover[0]);
                Rect rect1 = CombineRect(curRect, &parVars->m_cover[1]);
                ELEMTYPEREAL growth0 = CalcRectVolume(&rect0) - parVars->m_area[0];
                ELEMTYPEREAL growth1 = CalcRectVolume(&rect1) - parVars->m_area[1];
                ELEMTYPEREAL diff = growth1 - growth0;
                if (diff >= 0)
                {
                    group = 0;
                }
                else
                {
                    group = 1;
                    diff = -diff;
                }

                if (diff > biggestDiff)
                {
                    biggestDiff = diff;
                    chosen = index;
                    betterGroup = group;
                }
                else if ((diff == biggestDiff) && (parVars->m_count[group] < parVars->m_count[betterGroup]))
                {
                    chosen = index;
                    betterGroup = group;
                }
            }
        }
        Classify(chosen, betterGroup, parVars);
    }

    // If one group too full, put remaining rects in the other
    if ((parVars->m_count[0] + parVars->m_count[1]) < parVars->m_total)
    {
        if (parVars->m_count[0] >= parVars->m_total - parVars->m_minFill)
        {
            group = 1;
        }
        else
        {
            group = 0;
        }
        for (int index=0; index<parVars->m_total; ++index)
        {
            if (!parVars->m_taken[index])
            {
                Classify(index, group, parVars);
            }
        }
    }

    assert((parVars->m_count[0] + parVars->m_count[1]) == parVars->m_total);
    assert((parVars->m_count[0] >= parVars->m_minFill) && 
        (parVars->m_count[1] >= parVars->m_minFill));
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 분할에 따라, 브랜드치들을 버퍼에서 복사한다.
/// \param nodeA 노드
/// \param nodeB 노드
/// \param parVars 버퍼
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::LoadNodes(Node* nodeA, Node* nodeB, PartitionVars* parVars)
{
    assert(nodeA);
    assert(nodeB);
    assert(parVars);

    for (int index=0; index < parVars->m_total; ++index)
    {
        assert(parVars->m_partition[index] == 0 || parVars->m_partition[index] == 1);

        if (parVars->m_partition[index] == 0)
        {
            AddBranch(&parVars->m_branchBuf[index], nodeA, NULL);
        }
        else if (parVars->m_partition[index] == 1)
        {
            AddBranch(&parVars->m_branchBuf[index], nodeB, NULL);
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 트리의 분할을 찾을 때 쓰이는 변수 객체를 초기화한다.
/// \param parVars 변수 객체
/// \param maxRects 사격형의 갯수
/// \param minFill 최소 채우기 값
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::InitParVars(PartitionVars* parVars, int maxRects, int minFill)
{
    assert(parVars);

    parVars->m_count[0] = parVars->m_count[1] = 0;
    parVars->m_area[0] = parVars->m_area[1] = (ELEMTYPEREAL)0;
    parVars->m_total = maxRects;
    parVars->m_minFill = minFill;
    for (int index=0; index < maxRects; ++index)
    {
        parVars->m_taken[index] = false;
        parVars->m_partition[index] = -1;
    }
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 트리의 분할을 위한 시드 값을 계산한다.
/// \param parVars 트리의 분할을 찾을 때 쓰이는 변수 객체
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::PickSeeds(PartitionVars* parVars)
{
    int seed0, seed1;
    ELEMTYPEREAL worst, waste;
    ELEMTYPEREAL area[MAXNODES+1];

    for (int index=0; index<parVars->m_total; ++index)
    {
        area[index] = CalcRectVolume(&parVars->m_branchBuf[index].m_rect);
    }

    worst = -parVars->m_coverSplitArea - 1;
    for (int indexA=0; indexA < parVars->m_total-1; ++indexA)
    {
        for (int indexB = indexA+1; indexB < parVars->m_total; ++indexB)
        {
            Rect oneRect = CombineRect(&parVars->m_branchBuf[indexA].m_rect, &parVars->m_branchBuf[indexB].m_rect);
            waste = CalcRectVolume(&oneRect) - area[indexA] - area[indexB];
            if (waste > worst)
            {
                worst = waste;
                seed0 = indexA;
                seed1 = indexB;
            }
        }
    }

    Classify(seed0, 0, parVars);
    Classify(seed1, 1, parVars);
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 브랜치를 그룹별로 구분한다.
/// \param index 브랜치의 인덱스
/// \param group 그룹 인덱스
/// \param parVars 트리의 분할을 찾을 때 쓰이는 변수 객체
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::Classify(int index, int group, PartitionVars* parVars)
{
    assert(parVars);
    assert(!parVars->m_taken[index]);

    parVars->m_partition[index] = group;
    parVars->m_taken[index] = true;

    if (parVars->m_count[group] == 0)
    {
        parVars->m_cover[group] = parVars->m_branchBuf[index].m_rect;
    }
    else
    {
        parVars->m_cover[group] = CombineRect(&parVars->m_branchBuf[index].m_rect, &parVars->m_cover[group]);
    }
    parVars->m_area[group] = CalcRectVolume(&parVars->m_cover[group]);
    ++parVars->m_count[group];
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 바운딩 박스를 삭제한다.
/// \param rect 제거할 바운딩 박스
/// \param id 제거할 유저 데이터
/// \param root 루트 노드
/// \return bool 성공적으로 삭제한 경우에는 0을 반환하고, 해당 데이터를 찾지
/// 못한 경우에는 1을 반환한다.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
bool RTREE_QUALIFIER::RemoveRect(Rect* rect, const DATATYPE& id, Node** root)
{
    assert(rect && root);
    assert(*root);

    Node* tempNode;
    ListNode* reInsertList = NULL;

    if (!RemoveRectRec(rect, id, *root, &reInsertList))
    {
        // Found and deleted a data item
        // Reinsert any branches from eliminated nodes
        while(reInsertList)
        {
            tempNode = reInsertList->m_node;

            for (int index = 0; index < tempNode->m_count; ++index)
            {
                InsertRect(&(tempNode->m_branch[index].m_rect),
                    tempNode->m_branch[index].m_data,
                    root,
                    tempNode->m_level);
            }

            ListNode* remLNode = reInsertList;
            reInsertList = reInsertList->m_next;

            FreeNode(remLNode->m_node);
            FreeListNode(remLNode);
        }

        // Check for redundant root (not leaf, 1 child) and eliminate
        if ((*root)->m_count == 1 && (*root)->IsInternalNode())
        {
            tempNode = (*root)->m_branch[0].m_child;

            assert(tempNode);
            FreeNode(*root);
            *root = tempNode;
        }
        return false;
    }
    else
    {
        return true;
    }
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 루트가 아닌 다른 노드에서 바운딩 박스를 삭제한다.
///
/// RemoveRect 함수에서 호출한다. 트리를 따라내려가며, 노드를 합쳐야하는
/// 경우에는 합친다.
///
/// \param rect 제거할 바운딩 박스
/// \param id 제거할 유저 데이터
/// \param node 부모 노드
/// \param listNode 재삽입 리스트 객체
/// \return bool 성공적으로 삭제한 경우에는 0을 반환하고, 해당 데이터를 찾지
/// 못한 경우에는 1을 반환한다.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
bool RTREE_QUALIFIER::RemoveRectRec(Rect* rect, const DATATYPE& id, Node* node, ListNode** listNode)
{
    assert(rect && node && listNode);
    assert(node->m_level >= 0);

    if (node->IsInternalNode())  // not a leaf node
    {
        for (int index = 0; index < node->m_count; ++index)
        {
            if (Overlap(rect, &(node->m_branch[index].m_rect)))
            {
                if (!RemoveRectRec(rect, id, node->m_branch[index].m_child, listNode))
                {
                    if (node->m_branch[index].m_child->m_count >= MINNODES)
                    {
                        // child removed, just resize parent rect
                        node->m_branch[index].m_rect = NodeCover(node->m_branch[index].m_child);
                    }
                    else
                    {
                        // child removed, not enough entries in node, eliminate node
                        ReInsert(node->m_branch[index].m_child, listNode);
                        DisconnectBranch(node, index); // Must return after this call as count has changed
                    }
                    return false;
                }
            }
        }
        return true;
    }
    else // A leaf node
    {
        for (int index = 0; index < node->m_count; ++index)
        {
            if (node->m_branch[index].m_child == (Node*)id)
            {
                DisconnectBranch(node, index); // Must return after this call as count has changed
                return false;
            }
        }
        return true;
    }
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 두 바운딩 박스가 겹치는지의 여부를 반환한다.
/// \param rectA 바운딩 박스
/// \param rectB 바운딩 박스
/// \return bool 겹치는 경우 true, 겹치지 않는 경우 false
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
bool RTREE_QUALIFIER::Overlap(Rect* rectA, Rect* rectB) const
{
    assert(rectA && rectB);

    for (int index=0; index < NUMDIMS; ++index)
    {
        if (rectB->m_max[index] < rectA->m_min[index]  ||
            rectA->m_max[index] < rectB->m_min[index])
        {
            return false;
        }
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief 두 바운딩 박스가 겹치는지의 여부를 반환한다.
/// \param rectA 바운딩 박스
/// \param rectB 바운딩 박스
/// \return bool 겹치는 경우 true, 겹치지 않는 경우 false
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
bool RTREE_QUALIFIER::Contain(Rect* rectA, Rect* rectB) const
{
    assert(rectA && rectB);

    for (int index=0; index < NUMDIMS; ++index)
    {
        if(rectA->m_min[index] < rectB->m_min[index] &&
		   rectB->m_max[index] < rectA->m_max[index])
        {
         
        }
		else
		{
			return false;
		}
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief 노드를 재삽입 리스트에다 추가한다. 해당 노드와 브랜치들은 나중에
/// 트리에 새로이 추가한다.
/// \param node 노드
/// \param listNode 리스트 객체
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
void RTREE_QUALIFIER::ReInsert(Node* node, ListNode** listNode)
{
    ListNode* newListNode = AllocListNode();
    newListNode->m_node = node;
    newListNode->m_next = *listNode;
    *listNode = newListNode;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 실제 검색 함수
/// \param node 검색할 노드
/// \param rect 바운딩 박스
/// \param foundCount 찾은 데이터의 갯수
/// \param results 찾은 데이터를 집어넣을 컬렉션
/// \return bool 상위 함수에서 검색을 계속해야하는 경우에는 true, 검색을
/// 중단해야하는 경우에는 false.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
bool RTREE_QUALIFIER::Search(Node* node, Rect* rect, int& foundCount, RESULTS& results) const
{
    assert(node);
    assert(node->m_level >= 0);
    assert(rect);

    if (node->IsInternalNode()) // This is an internal node in the tree
    {
        for (int index=0; index < node->m_count; ++index)
        {
            if (Overlap(rect, &node->m_branch[index].m_rect))
            {
                if (!Search(node->m_branch[index].m_child, rect, foundCount, results))
                {
                    return false; // Don't continue searching
                }
            }
        }
    }
    else // This is a leaf node
    {
        for (int index=0; index < node->m_count; ++index)
        {
            if (Overlap(rect, &node->m_branch[index].m_rect))
            {
                ++foundCount;
                results.push_back(node->m_branch[index].m_data);
            }
        }
    }

    return true; // Continue searching
}

//////////////////////////////////////////////////////////////////////////////
/// \brief 실제 검색 함수
/// \param node 검색할 노드
/// \param rect 바운딩 박스
/// \param foundCount 찾은 데이터의 갯수
/// \param results 찾은 데이터를 집어넣을 컬렉션
/// \return bool 상위 함수에서 검색을 계속해야하는 경우에는 true, 검색을
/// 중단해야하는 경우에는 false.
//////////////////////////////////////////////////////////////////////////////
RTREE_TEMPLATE
bool RTREE_QUALIFIER::Contains(Node* node, Rect* rect, int& foundCount, RESULTS& results) const
{
    assert(node);
    assert(node->m_level >= 0);
    assert(rect);

    if (node->IsInternalNode()) // This is an internal node in the tree
    {
        for (int index=0; index < node->m_count; ++index)
        {
            if(Overlap(rect, &node->m_branch[index].m_rect))
            {
                if(!Contains(node->m_branch[index].m_child, rect, foundCount, results))
                {
                    return false; // Don't continue searching
                }
            }
        }
    }
    else // This is a leaf node
    {
        for (int index=0; index < node->m_count; ++index)
        {
            if(Contain(rect, &node->m_branch[index].m_rect))
            {
                ++foundCount;
                results.push_back(node->m_branch[index].m_data);
            }
        }
    }

    return true; // Continue searching
}

#undef RTREE_TEMPLATE
#undef RTREE_QUALIFIER

#endif