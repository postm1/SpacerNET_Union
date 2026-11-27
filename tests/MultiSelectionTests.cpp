// Compile the actual editor MultiSelect.cpp against a deterministic engine double.
// Its SetTrafoObjToWorld propagates parent motion to children, like the engine.
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>
#include "../SpacerNET_Union/MultiSelectionRules.hpp"
#define GOTHIC_ENGINE TestEngine
namespace TestEngine {
using std::min;
using std::max;
using uint = unsigned;
using bool_t = bool;
constexpr bool True = true, False = false, TRUE = true, FALSE = false;
constexpr std::nullptr_t Null = nullptr;
enum { VX, VY, VZ };
enum { VA, VB };
enum { KEY_LSHIFT, KEY_LALT, KEY_LCONTROL, Left, zRND_ALPHA_FUNC_ADD,
    zTRACERAY_STAT_POLY = 16, zTRACERAY_VOB_IGNORE = 32, SWM_MULTISELECT = 3, UI_ALL_VOBS_TREE_LIST = 4 };
struct zVEC2 {
    float n[2];
    zVEC2(float x=0,float y=0):n{x,y}{}
    float& operator[](int i){return n[i];}
    float operator[](int i)const{return n[i];}
    zVEC2 operator-(const zVEC2& b)const{return {n[0]-b[0],n[1]-b[1]};}
};
struct zVEC3 {
    float n[3];
    zVEC3(float x=0,float y=0,float z=0):n{x,y,z}{}
    float& operator[](int i){return n[i];}
    float operator[](int i)const{return n[i];}
    zVEC3 operator+(const zVEC3& b)const{return {n[0]+b[0],n[1]+b[1],n[2]+b[2]};}
    zVEC3 operator-(const zVEC3& b)const{return {n[0]-b[0],n[1]-b[1],n[2]-b[2]};}
    zVEC3& operator+=(const zVEC3& b){return *this=*this+b;}
    zVEC3& operator-=(const zVEC3& b){return *this=*this-b;}
    zVEC3 operator*(float f)const{return {n[0]*f,n[1]*f,n[2]*f};}
    float Dot(const zVEC3& b)const{return n[0]*b[0]+n[1]*b[1]+n[2]*b[2];}
    float Length_Sqr()const{return Dot(*this);}
    zVEC3 Cross(const zVEC3& b)const{return {n[1]*b[2]-n[2]*b[1],n[2]*b[0]-n[0]*b[2],n[0]*b[1]-n[1]*b[0]};}
    zVEC3& Normalize(){float f=std::sqrt(Length_Sqr());if(f)*this=*this*(1/f);return *this;}
};
struct zMAT4 {
    zVEC3 pos;
    int orientation=17;
    zVEC3 right={1,0,0},up={0,1,0},at={0,0,1};
    void SetTranslation(const zVEC3& value){pos=value;}
    zVEC3 GetRightVector()const{return right;}zVEC3 GetUpVector()const{return up;}zVEC3 GetAtVector()const{return at;}
    void SetRightVector(zVEC3 v){right=v;}void SetUpVector(zVEC3 v){up=v;}void SetAtVector(zVEC3 v){at=v;}
};
struct zCQuat {
    zMAT4 matrix;
    void QuatToMatrix4(zMAT4& out)const{out=matrix;out.pos=zVEC3();}
    void Matrix4ToQuat(zMAT4& value){matrix=value;matrix.pos=zVEC3();}
};
struct zCVisual {
    virtual ~zCVisual()=default;
    template<class T>T* CastTo(){return dynamic_cast<T*>(this);}
};
struct zCParticleFX:zCVisual{};
struct zCDecal:zCVisual{};
template<class T> struct zCArray {
    std::vector<T> data;
    int GetNumInList()const{return (int)data.size();}
    uint GetNum()const{return (uint)data.size();}
    void InsertEnd(T t){data.push_back(t);}
    T GetSafe(int i)const{return data.at(i);}
    T& operator[](int i){return data.at(i);}
    bool IsInList(T t)const{return std::find(data.begin(),data.end(),t)!=data.end();}
    bool HasEqual(T t)const{return IsInList(t);}
    void Clear(){data.clear();}
    void operator|=(T t){if(!HasEqual(t))data.push_back(t);}
    void operator^=(T t){data.erase(std::remove(data.begin(),data.end(),t),data.end());}
};
template<class T> struct zCTree {
    T* data=nullptr;
    zCTree* parent=nullptr;
    zCTree* child=nullptr;
    zCTree* sibling=nullptr;
    T* GetData(){return data;}
    zCTree* GetFirstChild(){return child;}
    zCTree* GetNextChild(){return sibling;}
};
struct zCWorld;
struct zCVob {
    static int classDef;
    zCTree<zCVob> node;
    zCTree<zCVob>* globalVobTreeNode=&node;
    zCWorld* world=nullptr;
    zMAT4 trafoObjToWorld;
    bool dontWriteIntoArchive=false, helper=false, collDetectionDynamic=true, collDetectionStatic=false, bbox=false;
    int refs=1, pfxUpdates=0;
    zVEC3 forward={0,0,1}, right={1,0,0};
    zCVisual* visual=nullptr;
    bool showVisual=true;
    zCVisual* GetVisual(){return visual;}
    bool GetShowVisual(){return showVisual;}
    bool GetCollDetDyn(){return collDetectionDynamic;}
    zCVob(){node.data=this;}
    virtual ~zCVob()=default;
    template<class T>T* CastTo(){return dynamic_cast<T*>(this);}
    zCWorld* GetHomeWorld(){return world;}
    zCVob* GetParentVob(){return node.parent?node.parent->data:nullptr;}
    zVEC3 GetPositionWorld(){return trafoObjToWorld.pos;}
    zVEC3 GetAtVectorWorld(){return forward;}
    zVEC3 GetRightVectorWorld(){return right;}
    void SetDrawBBox3D(bool value){bbox=value;}
    void SetCollDetDyn(bool value){collDetectionDynamic=value;}
    void SetCollDetStat(bool value){collDetectionStatic=value;}
    void AddRef(){++refs;}
    void Release(){--refs;}
    void Propagate(const zVEC3& delta){
        trafoObjToWorld.pos+=delta;
        for(auto n=node.child;n;n=n->sibling)n->data->Propagate(delta);
    }
    static zVEC3 RebaseDirection(zVEC3 vector,const zMAT4& oldParent,const zMAT4& newParent){
        return newParent.right*vector.Dot(oldParent.right)
            +newParent.up*vector.Dot(oldParent.up)+newParent.at*vector.Dot(oldParent.at);
    }
    void PropagateTransform(const zMAT4& oldParent,const zMAT4& newParent){
        trafoObjToWorld.pos=newParent.pos+RebaseDirection(trafoObjToWorld.pos-oldParent.pos,oldParent,newParent);
        trafoObjToWorld.right=RebaseDirection(trafoObjToWorld.right,oldParent,newParent);
        trafoObjToWorld.up=RebaseDirection(trafoObjToWorld.up,oldParent,newParent);
        trafoObjToWorld.at=RebaseDirection(trafoObjToWorld.at,oldParent,newParent);
        for(auto n=node.child;n;n=n->sibling)n->data->PropagateTransform(oldParent,newParent);
    }
    void SetTrafoObjToWorld(const zMAT4& value){
        zMAT4 before=trafoObjToWorld;
        PropagateTransform(before,value);
        trafoObjToWorld=value;
    }
};
int zCVob::classDef=0;
struct zCVobLevelCompo:zCVob{};
struct oCItem:zCVob{};
struct zCVobSpot:zCVob{};
struct zCVobWaypoint:zCVob{};
struct zCCSCamera:zCVob{int refreshes=0;void Refresh(){++refreshes;}};
struct zCCamTrj_KeyFrame:zCVob{zCCSCamera* cscam=nullptr;};
struct oCMOB:zCVob{};
struct oCMobInter:oCMOB{};
struct zCVobLight:zCVob{};
struct zCVobSound:zCVob{};
struct zCTrigger:zCVob{};
struct zCZone:zCVob{};
struct oCZoneMusic:zCZone{};
struct zCMover:zCTrigger{struct Key{zVEC3 pos;zCQuat quat;Key(zVEC3 p={}):pos(p){}};zCArray<Key> keyframeList;};
struct WayPoint{int updates=0;void UpdatePositionWorld(){++updates;}};
struct WayNet{std::map<zCVobWaypoint*,WayPoint> points;WayPoint* SearchWaypoint(zCVobWaypoint* wp){return &points[wp];}};
struct zCWorld {
    zCArray<zCVob*> live;
    WayNet waynet;
    bool obstructed=false;
    void SearchVobListByBaseClass(int,zCArray<zCVob*>& out,int){out=live;}
    bool TraceRayNearestHit(zVEC3,zVEC3,zCVob*,int){return obstructed;}
    WayNet* GetWayNet(){return &waynet;}
};
using oCWorld=zCWorld;
struct Game {zCWorld* world=nullptr;zCVob* camera=nullptr;zCWorld* GetWorld(){return world;}zCVob* GetCameraVob(){return camera;}};
Game game;
Game* ogame=&game;
struct zCView {
    void InsertBack(const char*){}void SetAlphaBlendFunc(int){}void SetTransparency(int){}
    void SetPos(float,float){}void SetSize(float,float){}void InsertItem(zCView*){}void RemoveItem(zCView*){}
};
zCView screenValue;
zCView* screen=&screenValue;
struct Input {bool right=false;bool GetMouseButtonPressedRight(){return right;}};
Input input;
Input* zinput=&input;
struct Timer {float frameTimeFloat=16;};
Timer timer;
Timer* ztimer=&timer;
std::unordered_set<int> rawKeys;
bool mouseLeft=false;
bool groupInputBlocked=false;
bool zKeyPressed(int key){return rawKeys.count(key)!=0;}
bool zMouseKeyPressed(int){return mouseLeft;}
struct KeyManager {
    std::unordered_set<std::string> pressed;
    bool KeyPressed(const char* key,bool clear=false,bool=false){
        bool result=pressed.count(key)!=0;if(result&&clear)pressed.erase(key);return result;
    }
} keys;
struct Printer{void PrintRed(const char*){}}print;
const char* GetLang(const char* s){return s;}
bool IsSpacerVob(zCVob* vob){return vob->helper;}
void GetProjection(int& x,int& y,zVEC3 pos){x=(int)pos[VX];y=(int)pos[VY];}
void UpdatePfxBbox(zCVob* vob){++vob->pfxUpdates;}
bool CanChangeVobParent(zCVob*,zCVob*){return true;}
void HandleParentChange(zCVob*,zCVob*,bool=true){}
struct Options {
    std::map<std::string,int> values;
    int GetIntVal(const char* key){return values[key];}
    void SetIntVal(const char* key,int value){values[key]=value;}
};
struct App {
    Options options;
    zCArray<zCVob*> SelectedVobs;
    bool isExit=false,g_bIsPlayingGame=false,focused=true,isVobParentChange=false,dirty=false,dx11=true;
    int mode=SWM_MULTISELECT;
    zVEC2 mouse={10,10};
    zCVob* globalParent=nullptr;
    zCVob* vobToCopy=nullptr;
    struct {bool cameraRun=false;}camMan;
    struct {void toggleUIElement(int,bool){}}exports;
    bool GameFocused(){return focused;}
    bool IsDx11Active(){return dx11;}
    zVEC2 GetMousePosVirt(){return mouse;}
    int GetPickMode(){return mode;}
    void SetChangesWereMade(bool value){dirty=value;}
    void RenderDx11_Bbox(zCVob*){}
    void RemoveVob(zCVob* vob);
}theApp;
}
#include "../SpacerNET_Union/MultiSelect.h"
#include "../SpacerNET_Union/MultiSelect.cpp"
namespace TestEngine {
bool MultiSelect::InputBlocked(){return groupInputBlocked;}
void App::RemoveVob(zCVob* vob){Selector.ForgetVob(vob);game.world->live^=vob;vob->world=nullptr;}
}
using namespace TestEngine;
void require(bool condition,const char* message){if(!condition)throw std::runtime_error(message);}
bool equal(zVEC3 a,zVEC3 b){return (a-b).Length_Sqr()<0.00001f;}
struct Scene {
    zCWorld world;
    zCVob camera;
    std::vector<std::unique_ptr<zCVob>> storage;
    Scene(){
        theApp=App();keys.pressed.clear();rawKeys.clear();mouseLeft=false;input.right=false;groupInputBlocked=false;
        game.world=&world;game.camera=&camera;camera.world=&world;
        theApp.options.values["vobTransSpeed"]=100000;
        Selector.Reset();
    }
    ~Scene(){Selector.Reset();game.world=nullptr;}
    template<class T=zCVob>T* add(zVEC3 pos={},zCVob* parent=nullptr){
        auto ptr=new T();storage.emplace_back(ptr);ptr->world=&world;ptr->trafoObjToWorld.pos=pos;
        ptr->node.parent=parent?&parent->node:nullptr;
        if(parent){ptr->node.sibling=parent->node.child;parent->node.child=&ptr->node;}
        world.live.InsertEnd(ptr);return ptr;
    }
    void select(zCVob* p){Selector.AddToSelection(p);}
};
int main(){
    try{
        int checks=0;
        {
            Scene s;auto a=s.add({100,100,10});s.select(a);
            mouseLeft=true;Selector.DoPick();mouseLeft=false;Selector.DoPick();
            require(Selector.GetCount()==1,"A zero-area click never clears the group in an already active viewport");
            mouseLeft=true;theApp.mouse={15,15};Selector.DoPick();
            mouseLeft=false;theApp.mouse={25,25};Selector.DoPick();
            require(Selector.GetCount()==0,"A deliberate empty rectangle still clears the group");
            checks+=2;
        }
        {
            Scene s;auto a=s.add({100,100,10});auto b=s.add({110,110,10});auto c=s.add({20,20,10});
            Selector.ChangeSelection(a,0);Selector.ChangeSelection(b,1);
            theApp.focused=false;groupInputBlocked=true;mouseLeft=true;Selector.ManageKeys();
            mouseLeft=false;Selector.ManageKeys();
            theApp.focused=true;groupInputBlocked=false;mouseLeft=true;theApp.mouse={15,15};
            Selector.ManageKeys();
            require(!Selector.Locked&&Selector.GetCount()==2,"Activating the viewport after list selection does not start a rectangle");
            keys.pressed.insert("VOB_TRANS_FORWARD");theApp.mouse={25,25};Selector.ManageKeys();
            require(!Selector.Locked&&equal(a->GetPositionWorld(),{100,100,10}),"Held activation click cannot select or move objects");
            keys.pressed.clear();mouseLeft=false;Selector.ManageKeys();
            require(Selector.GetCount()==2&&a->bbox&&b->bbox,"Releasing the focus click preserves the entire group");
            theApp.mouse={15,15};mouseLeft=true;Selector.ManageKeys();
            require(Selector.Locked,"The next mouse gesture starts normal rectangle selection");
            theApp.mouse={25,25};mouseLeft=false;Selector.ManageKeys();
            require(Selector.GetCount()==1&&theApp.SelectedVobs.HasEqual(c),"A deliberate rectangle still replaces the group");
            groupInputBlocked=true;Selector.ManageKeys();groupInputBlocked=false;
            Selector.ManageKeys(); // Focus restored without holding LMB (e.g. with RMB).
            mouseLeft=true;Selector.ManageKeys();
            require(Selector.Locked,"Returning without a left click does not consume the next selection gesture");
            theApp.focused=false;Selector.ManageKeys();
            require(!Selector.Locked&&Selector.GetCount()==1,"Losing focus cancels an unfinished rectangle without clearing the group");
            theApp.focused=true;Selector.ManageKeys();mouseLeft=false;Selector.ManageKeys();
            require(Selector.GetCount()==1,"Dragging back into the viewport with LMB held preserves selection");
            checks+=8;
        }
        {
            Scene s;auto a=s.add();Selector.ChangeSelection(a,0);
            // A list callback between render frames must arm the focus protection itself.
            mouseLeft=true;Selector.DoPick();mouseLeft=false;Selector.DoPick();
            require(!Selector.Locked&&Selector.GetCount()==1,"Fast return from list is protected even without a blocked render frame");
            groupInputBlocked=true;Selector.DoPick();
            groupInputBlocked=false;input.right=true;Selector.ManageKeys();
            input.right=false;mouseLeft=true;Selector.DoPick();
            require(Selector.Locked,"Camera activation with RMB leaves subsequent LMB selection available");
            Selector.Reset();Selector.AddToSelection(a);
            mouseLeft=true;Selector.DoPick();
            require(Selector.Locked,"World/reset clears stale activation state");
            checks+=3;
        }
        {
            Scene s;auto a=s.add({10,0,0});auto b=s.add({20,0,0});auto item=s.add<oCItem>();
            require(Selector.ChangeSelection(a,0)==1&&a->bbox,"List click replaces selection");
            theApp.focused=false;groupInputBlocked=true;
            require(Selector.ChangeSelection(b,1)==2&&a->bbox&&b->bbox,"List can add without viewport focus");
            require(Selector.ChangeSelection(b,1)==2,"Adding an already selected row is idempotent");
            require(Selector.Translate(3,0,0)==2&&equal(a->GetPositionWorld(),{13,0,0})
                &&equal(b->GetPositionWorld(),{23,0,0}),"List selection uses group transforms");
            require(Selector.UndoTranslation()==1&&equal(a->GetPositionWorld(),{10,0,0}),"List transform undo");
            require(Selector.ChangeSelection(a,2)==1&&!a->bbox&&b->bbox,"List removes one member");
            require(Selector.ChangeSelection(a,2)==1,"Removing an unselected row keeps the group");
            require(Selector.ChangeSelection(a,0)==1&&a->bbox&&!b->bbox,"Plain list click clears the previous group");
            require(Selector.ChangeSelection(nullptr,0)==-1
                &&Selector.ChangeSelection(reinterpret_cast<zCVob*>(1),1)==-1
                &&Selector.ChangeSelection(a,99)==-1&&Selector.GetCount()==1,"Invalid addresses/actions leave selection intact");
            Selector.SetFilter(SpacerSelection::OnlyVobs);
            require(Selector.ChangeSelection(item,0)==-1&&Selector.GetCount()==1,"Rejected filter does not clear the group");
            auto helper=s.add();helper->helper=true;
            require(Selector.ChangeSelection(helper,1)==-1,"Helpers cannot be selected from a list");
            theApp.g_bIsPlayingGame=true;
            require(Selector.ChangeSelection(b,1)==-1&&Selector.ChangeSelection(a,2)==-1,"Play mode blocks list edits");
            theApp.g_bIsPlayingGame=false;theApp.mode=0;
            require(Selector.ChangeSelection(b,1)==-1,"List API requires group mode");
            theApp.mode=SWM_MULTISELECT;theApp.camMan.cameraRun=true;
            require(Selector.ChangeSelection(b,1)==-1,"Camera playback blocks list edits");
            theApp.camMan.cameraRun=false;
            Selector.Translate(1,0,0,true);Selector.ChangeSelection(b,1);
            require(!activeGroupMove.HasChanges()&&groupMoveUndo.size()==1,"List click finishes a keyboard gesture");
            Selector.UndoTranslation();
            require(equal(a->GetPositionWorld(),{10,0,0})&&equal(b->GetPositionWorld(),{20,0,0}),"Selection changes preserve previous undo targets");
            s.world.live^=b;
            require(Selector.ChangeSelection(b,1)==-1&&Selector.GetCount()==1,"Stale list rows cannot be added");
            checks+=17;
        }
        {
            Scene s;
            auto plain=s.add();auto item=s.add<oCItem>();auto mover=s.add<zCMover>();
            auto child=s.add({},mover);auto deep=s.add({},child);auto wp=s.add<zCVobWaypoint>();
            auto fp=s.add<zCVobSpot>();auto special=s.add<zCCSCamera>();auto helper=s.add();helper->helper=true;
            auto level=s.add<zCVobLevelCompo>();auto temporary=s.add();temporary->dontWriteIntoArchive=true;
            Selector.SetFilter(0);
            require(Selector.MatchesFilter(plain)&&Selector.MatchesFilter(item)&&Selector.MatchesFilter(wp),"All includes editable types");
            require(!Selector.MatchesFilter(helper)&&!Selector.MatchesFilter(level)&&!Selector.MatchesFilter(temporary),"Helpers excluded");
            s.select(plain);s.select(item);s.select(mover);s.select(deep);
            Selector.SetFilter(1);
            require(Selector.GetCount()==2&&!Selector.MatchesFilter(deep),"Mover subtree excluded at every depth");
            Selector.SetFilter(2);
            require(!Selector.MatchesFilter(wp)&&!Selector.MatchesFilter(fp)&&Selector.MatchesFilter(item),"Waypoint filter");
            Selector.SetFilter(3);
            require(Selector.GetCount()==1&&Selector.MatchesFilter(item)&&!Selector.MatchesFilter(plain),"Item-only prunes old selection");
            Selector.SetFilter(4);
            require(Selector.GetCount()==0&&Selector.MatchesFilter(plain)&&Selector.MatchesFilter(wp)
                &&Selector.MatchesFilter(fp)&&Selector.MatchesFilter(special)&&!Selector.MatchesFilter(item)
                &&!Selector.MatchesFilter(mover),"VOB means any editable VOB except items/movers");
            checks+=6;
        }
        {
            Scene s;
            auto parent=s.add({10,20,30});
            auto child=s.add({11,22,33},parent);
            auto fixed=s.add<zCMover>({5,6,7},parent);fixed->keyframeList.InsertEnd({{50,60,70}});
            auto wp=s.add<zCVobWaypoint>({4,5,6},parent);
            auto spline=s.add<zCCSCamera>();
            auto key=s.add<zCCamTrj_KeyFrame>({2,3,4},parent);key->cscam=spline;
            s.select(parent);s.select(child);s.select(wp);s.select(key);
            require(Selector.Translate(100,200,300)==4,"Group moved");
            require(equal(parent->GetPositionWorld(),{110,220,330})&&equal(child->GetPositionWorld(),{111,222,333}),"Child moved once");
            require(equal(fixed->GetPositionWorld(),{5,6,7})&&equal(fixed->keyframeList[0].pos,{50,60,70}),"Unselected mover and route remain fixed");
            require(s.world.waynet.points[wp].updates==1&&spline->refreshes==1,"Waypoint and camera refreshed");
            require(parent->collDetectionDynamic&&!parent->collDetectionStatic&&theApp.dirty,"Collision flags and dirty state");
            require(Selector.UndoTranslation()==1&&equal(child->GetPositionWorld(),{11,22,33})
                &&equal(fixed->GetPositionWorld(),{5,6,7})&&parent->trafoObjToWorld.orientation==17,"Atomic undo");
            checks+=6;
        }
        {
            Scene s;auto mover=s.add<zCMover>({1,2,3});mover->keyframeList.InsertEnd({{10,20,30}});
            mover->keyframeList.InsertEnd({{40,50,60}});s.select(mover);
            Selector.Translate(1,2,3,true);Selector.Translate(4,5,6,true);Selector.EndTranslation();
            require(equal(mover->GetPositionWorld(),{6,9,12})&&equal(mover->keyframeList[1].pos,{45,57,69}),"Continuous movement uses cumulative snapshot delta");
            require(groupMoveUndo.size()==1,"One gesture creates one undo entry");
            Selector.UndoTranslation();
            require(equal(mover->GetPositionWorld(),{1,2,3})&&equal(mover->keyframeList[1].pos,{40,50,60}),"Mover route undo");
            checks+=3;
        }
        {
            Scene s;auto p=s.add();s.select(p);
            require(Selector.Translate(std::numeric_limits<float>::quiet_NaN(),0,0)==-3
                &&Selector.Translate(std::numeric_limits<float>::infinity(),0,0)==-3
                &&Selector.Translate(1000001,0,0)==-3&&!Selector.CanUndoTranslation(),"Nonfinite and out-of-range input rejected");
            for(int i=0;i<35;++i)Selector.Translate(1,0,0);
            require(groupMoveUndo.size()==32,"History bounded");
            for(int i=0;i<32;++i)require(Selector.UndoTranslation()==1,"Undo available");
            require(equal(p->GetPositionWorld(),{3,0,0})&&Selector.UndoTranslation()==0,"History boundary");
            checks+=3;
        }
        {
            Scene s;auto parent=s.add();auto child=s.add({},parent);s.select(parent);
            theApp.options.values["checkBoxBlockComplexVob"]=1;
            require(Selector.Translate(10,0,0)==-2&&equal(parent->GetPositionWorld(),{}),"Compound protection is atomic");
            theApp.options.values["checkBoxBlockComplexVob"]=0;
            Selector.Translate(10,0,0);
            s.add({},parent);
            require(Selector.UndoTranslation()==-1&&equal(parent->GetPositionWorld(),{10,0,0}),"Undo rejects changed hierarchy without partial writes");
            s.select(child);Selector.Translate(1,0,0);theApp.RemoveVob(child);
            require(!Selector.CanUndoTranslation()&&!theApp.SelectedVobs.HasEqual(child),"Deletion clears references and undo");
            checks+=3;
        }
        {
            Scene s;auto a=s.add({20,20,10});auto b=s.add<zCVobWaypoint>({30,30,10});
            auto behind=s.add({20,20,-10});auto outside=s.add({200,200,10});
            Selector.Bounds[0]={0,0};Selector.Bounds[1]={100,100};
            s.world.obstructed=true;
            Selector.PickBounds();require(Selector.GetCount()==0,"Visible-only respects occlusion");
            theApp.options.values["multiSelectionThroughWalls"]=1;
            Selector.PickBounds();require(Selector.GetCount()==2 && theApp.SelectedVobs.HasEqual(a)
                && theApp.SelectedVobs.HasEqual(b) && !theApp.SelectedVobs.HasEqual(behind)
                && !theApp.SelectedVobs.HasEqual(outside),"Rectangle includes invisible/waypoint candidates but not behind-camera objects");
            rawKeys.insert(KEY_LALT);Selector.PickBounds();require(Selector.GetCount()==0,"Alt subtract");
            rawKeys.clear();s.world.obstructed=false;theApp.dx11=false;
            Selector.PickBounds();require(Selector.GetCount()==2,"Same candidate enumeration for both renderers");
            checks+=4;
        }
        {
            Scene s;auto a=s.add();auto b=s.add<oCItem>({500,0,0});s.select(a);s.select(b);
            game.camera->forward={0,-0.99f,0.1f};
            keys.pressed.insert("VOB_TRANS_FORWARD");
            Selector.ManageKeys();Selector.ManageKeys();keys.pressed.clear();Selector.ManageKeys();
            require(equal(b->GetPositionWorld()-a->GetPositionWorld(),{500,0,0}),"Different object types keep relative spacing");
            require(groupMoveUndo.size()==1&&std::fabs(a->GetPositionWorld()[VZ]-32)<0.001f,"Keyboard normalizes pitched-camera basis and coalesces gesture");
            keys.pressed.insert("MULTI_UNDO");Selector.ManageKeys();
            require(equal(a->GetPositionWorld(),{}),"Keyboard undo");
            groupInputBlocked=true;keys.pressed.insert("VOB_DELETE");Selector.ManageKeys();
            require(s.world.live.GetNumInList()==2,"Main-window focus with an active numeric field must not delete");
            groupInputBlocked=false;theApp.focused=false;Selector.ManageKeys();
            require(s.world.live.GetNumInList()==2,"UI text focus does not delete selected objects");
            checks+=4;
        }
        {
            Scene s;auto p=s.add();s.select(p);Selector.Translate(5,0,0);
            zCWorld other;game.world=&other;
            require(Selector.UndoTranslation()==-1&&equal(p->GetPositionWorld(),{5,0,0}),"World switch rejects stale undo");
            game.world=&s.world;
            theApp.g_bIsPlayingGame=true;
            require(Selector.Translate(1,0,0)==-1,"Play mode blocks editing");
            checks+=2;
        }

        for(unsigned seed=1;seed<=20;++seed) {
            Scene s; unsigned random=seed;
            auto next=[&](){random=random*1664525u+1013904223u;return random;};
            std::vector<zCVob*> nodes;
            std::vector<zVEC3> original,originalKeys;
            std::vector<bool> picked;
            for(int i=0;i<60;++i) {
                zCVob* parent=i?nodes[next()%nodes.size()]:nullptr;
                zVEC3 position((float)(next()%200),(float)(next()%200),(float)(next()%200));
                auto vob=(next()%4)==0?(zCVob*)s.add<zCMover>(position,parent):s.add(position,parent);
                if(auto mover=vob->CastTo<zCMover>())mover->keyframeList.InsertEnd({position+zVEC3(3,4,5)});
                nodes.push_back(vob);original.push_back(position);originalKeys.push_back(position+zVEC3(3,4,5));
                bool select=i==0||(next()%3)==0;picked.push_back(select);if(select)s.select(vob);
            }
            require(Selector.Translate(7,-3,11)>0,"Randomized hierarchy move");
            for(size_t i=0;i<nodes.size();++i) {
                zVEC3 delta=picked[i]?zVEC3(7,-3,11):zVEC3();
                require(equal(nodes[i]->GetPositionWorld(),original[i]+delta),"Selected-only invariant across deep mixed hierarchies");
                if(auto mover=nodes[i]->CastTo<zCMover>())
                    require(equal(mover->keyframeList[0].pos,originalKeys[i]+delta),"Randomized mover route invariant");
            }
            require(Selector.UndoTranslation()==1,"Randomized undo");
            for(size_t i=0;i<nodes.size();++i)
                require(equal(nodes[i]->GetPositionWorld(),original[i]),"Randomized exact restore");
            ++checks;
        }

        {
            Scene s;auto a=s.add({10,0,0});auto b=s.add({-10,0,0});
            auto fixed=s.add({11,2,3},a);s.select(a);s.select(b);
            require(Selector.Rotate(0,90,0)==2,"Rotate selected pair");
            require(equal(a->GetPositionWorld(),{0,0,-10})&&equal(b->GetPositionWorld(),{0,0,10}),"Shared center rotation");
            require(equal(a->trafoObjToWorld.GetAtVector(),{1,0,0}),"Object orientation rotated");
            require(equal(fixed->GetPositionWorld(),{11,2,3})
                &&equal(fixed->trafoObjToWorld.GetAtVector(),{0,0,1}),"Unselected child keeps full world transform");
            require(Selector.UndoTranslation()==1&&equal(a->GetPositionWorld(),{10,0,0})
                &&equal(a->trafoObjToWorld.GetAtVector(),{0,0,1}),"Rotation undo restores position and orientation");
            checks+=5;
        }
        {
            Scene s;auto parent=s.add({-10,0,0});auto child=s.add({10,0,0},parent);
            s.select(parent);s.select(child);
            Selector.Rotate(0,0,90);
            require(equal(parent->GetPositionWorld(),{0,-10,0})&&equal(child->GetPositionWorld(),{0,10,0}),"Selected parent and child rotated once");
            require(equal(child->trafoObjToWorld.GetRightVector(),{0,1,0}),"Child orientation rotated once");
            Selector.UndoTranslation();
            require(equal(child->GetPositionWorld(),{10,0,0}),"Hierarchy rotation undo");
            checks+=3;
        }
        {
            Scene s;auto mover=s.add<zCMover>({10,0,0});
            mover->keyframeList.InsertEnd({{12,0,0}});
            s.select(mover);Selector.Rotate(0,90,0);
            require(equal(mover->keyframeList[0].pos,{10,0,-2}),"Mover key position rotated about group pivot");
            require(equal(mover->keyframeList[0].quat.matrix.GetAtVector(),{1,0,0}),"Mover key quaternion rotated");
            Selector.UndoTranslation();
            require(equal(mover->keyframeList[0].pos,{12,0,0})
                &&equal(mover->keyframeList[0].quat.matrix.GetAtVector(),{0,0,1}),"Mover key orientation undo");
            checks+=3;
        }
        {
            Scene s;auto root=s.add();auto mover=s.add<zCMover>({5,6,7},root);
            mover->keyframeList.InsertEnd({{6,7,8}});s.select(root);
            Selector.Rotate(25,45,65);
            require(equal(mover->GetPositionWorld(),{5,6,7})&&equal(mover->keyframeList[0].pos,{6,7,8})
                &&equal(mover->keyframeList[0].quat.matrix.GetAtVector(),{0,0,1}),"Unselected mover and path stay fixed during parent rotation");
            checks+=1;
        }
        {
            Scene s;auto a=s.add({10,0,0});auto b=s.add({-10,0,0});s.select(a);s.select(b);
            Selector.Translate(100,0,0);Selector.Rotate(0,45,0,true);Selector.Rotate(0,45,0,true);Selector.EndTranslation();
            require(groupMoveUndo.size()==2&&equal(a->GetPositionWorld(),{100,0,-10}),"Translation then continuous rotation form separate gestures");
            Selector.UndoTranslation();require(equal(a->GetPositionWorld(),{110,0,0}),"Undo rotation only");
            Selector.UndoTranslation();require(equal(a->GetPositionWorld(),{10,0,0}),"Undo preceding translation");
            require(Selector.Rotate(3601,0,0)==-3
                &&Selector.Rotate(std::numeric_limits<float>::quiet_NaN(),0,0)==-3,"Invalid rotation rejected");
            checks+=4;
        }
        {
            Scene s;auto p=s.add();s.select(p);
            keys.pressed.insert("VOB_ROTATE");Selector.ManageKeys();
            require(theApp.options.GetIntVal("multiSelectionTransformMode")==1,"Keyboard selects rotation mode");
            theApp.options.SetIntVal("vobRotSpeed",1000);
            keys.pressed.insert("VOB_ROT_VERT_RIGHT");Selector.ManageKeys();Selector.ManageKeys();
            keys.pressed.clear();Selector.ManageKeys();
            require(groupMoveUndo.size()==1&&!activeGroupMove.HasChanges(),"Keyboard rotation coalesced into one operation");
            keys.pressed.insert("MULTI_UNDO");Selector.ManageKeys();
            require(equal(p->trafoObjToWorld.GetAtVector(),{0,0,1}),"Keyboard rotation undo");
            keys.pressed.insert("VOB_TRANSLATE");Selector.ManageKeys();
            require(theApp.options.GetIntVal("multiSelectionTransformMode")==0,"Keyboard returns to translation");
            checks+=4;
        }
        {
            Scene s;
            auto plain=s.add();auto wp=s.add<zCVobWaypoint>();auto fp=s.add<zCVobSpot>();
            auto mob=s.add<oCMOB>();auto interactive=s.add<oCMobInter>();auto light=s.add<zCVobLight>();
            auto sound=s.add<zCVobSound>();auto trigger=s.add<zCTrigger>();auto zone=s.add<zCZone>();
            auto music=s.add<oCZoneMusic>();auto particle=s.add();auto decal=s.add();
            auto visible=s.add();auto noCollision=s.add();noCollision->collDetectionDynamic=false;
            zCParticleFX pfx;zCDecal texture;zCVisual model;
            particle->visual=&pfx;decal->visual=&texture;visible->visual=&model;
            struct Case{int filter;zCVob* accepted;zCVob* rejected;};
            std::vector<Case> cases={
                {SpacerSelection::Points,fp,plain},{SpacerSelection::Waypoints,wp,fp},
                {SpacerSelection::Freepoints,fp,wp},{SpacerSelection::Mobs,mob,plain},
                {SpacerSelection::DynamicCollision,plain,noCollision},
                {SpacerSelection::InteractiveMobs,interactive,trigger},
                {SpacerSelection::Lights,light,plain},{SpacerSelection::Sounds,sound,plain},
                {SpacerSelection::WithoutPfx,plain,particle},{SpacerSelection::Invisible,plain,visible},
                {SpacerSelection::WithoutDecals,plain,decal},{SpacerSelection::WithoutDecalsAndPfx,plain,particle},
                {SpacerSelection::Decals,decal,particle},{SpacerSelection::Particles,particle,decal},
                {SpacerSelection::Triggers,trigger,interactive},{SpacerSelection::Zones,zone,plain},
                {SpacerSelection::MusicZones,music,zone}
            };
            require(SpacerSelection::FilterCount==22,"All legacy selection and VOB-type filters covered");
            for(const auto& test:cases) {
                Selector.SetFilter(test.filter);
                require(Selector.MatchesFilter(test.accepted)&&!Selector.MatchesFilter(test.rejected),"Extended filter semantics");
                ++checks;
            }
            Selector.SetFilter(SpacerSelection::Points);
            require(Selector.MatchesFilter(wp)&&Selector.MatchesFilter(fp),"Combined WP and FP");
            ++checks;
        }
        std::cout<<"PASS: "<<checks<<" group-selection regression scenarios\n";
        return 0;
    }catch(const std::exception& e){std::cerr<<"FAIL: "<<e.what()<<"\n";return 1;}
}
