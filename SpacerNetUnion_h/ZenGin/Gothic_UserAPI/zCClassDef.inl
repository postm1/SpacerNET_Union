// Supported with union (c) 2020 Union team

// User API for zCClassDef
// Add your methods here



static int					GetNum() { return classDefList->GetNum(); };
static zCClassDef*			GetClassDefByIndex(int i) { return classDefList->GetSafe(i); };
const zSTRING&				GetClassName_() const { return className; };
const zSTRING&				GetBaseClassName() const { return baseClassName; };
zCClassDef*					GetBaseClassDef() const { return baseClassDef; };
zBOOL						IsScriptedClass() const { return !scriptClassName.IsEmpty(); };

zBOOL						IsAbstractClass() const { return (createNewInstance == 0); };