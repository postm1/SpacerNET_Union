// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	static Array<TSTACKTYPE*> arrStack;

	UAPI void Stack_Push(TSTACKTYPE* value) {
		arrStack += value;
		//cmd << "+ " << "push " << arrStack.GetNum() << "  " << value->GetTypeName() << endl;
	}

	UAPI void Stack_Pop() {
		if (arrStack.GetNum() == 0)
			Message::Fatal("Stack num already 0");

		TSTACKTYPE* value = arrStack.GetLast();
		arrStack.Remove(value);

		//cmd << "- " << "pop  " << arrStack.GetNum() << "  " << value->GetTypeName() << endl;
		value->Destroy();
	}

	UAPI void Stack_PushString(const char* v) {
		TSTRING* value = new TSTRING();
		value->value = v;
		Stack_Push(value);
	}

	UAPI void Stack_PushStringWide(const wchar_t* v) {

		TSTRINGW* value = new TSTRINGW();
		value->value = v;

		Stack_Push(value);
	}

	UAPI void Stack_PushInt(int v) {
		TINT* value = new TINT();
		value->value = v;
		Stack_Push(value);
	}

	UAPI void Stack_PushLong(int64 v) {
		TLONG* value = new TLONG();
		value->value = v;
		Stack_Push(value);
	}

	UAPI void Stack_PushUInt(uint v) {
		TUINT* value = new TUINT();
		value->value = v;
		Stack_Push(value);
	}

	UAPI void Stack_PushULong(uint64 v) {
		TULONG* value = new TULONG();
		value->value = v;
		Stack_Push(value);
	}

	UAPI void Stack_PushFloat(float v) {
		TFLOAT* value = new TFLOAT();
		value->value = v;
		Stack_Push(value);
	}

	UAPI void Stack_PushDouble(double v) {
		TDOUBLE* value = new TDOUBLE();
		value->value = v;
		Stack_Push(value);
	}

	UAPI char* Stack_PeekString_S() {
		TSTRING* value = dynamic_cast<TSTRING*>(arrStack.GetLast());
		if (!value)
			Message::Fatal("Value is not TSTRING");

		return value->value.ToChar();
	}

	UAPI wchar_t* Stack_PeekString_SW() {
		TSTRINGW* value = dynamic_cast<TSTRINGW*>(arrStack.GetLast());
		if (!value)
			Message::Fatal("Value is not TSTRINGW");

		return value->value.ToWChar();
	}

	UAPI string Stack_PeekString() {
		string value = Stack_PeekString_S();
		Stack_Pop();
		return value;
	}

	UAPI CStringW Stack_PeekStringW() {
		CStringW value = Stack_PeekString_SW();
		Stack_Pop();
		return value;
	}

	UAPI int Stack_PeekInt() {
		TINT* value = dynamic_cast<TINT*>(arrStack.GetLast());
		if (!value)
			Message::Fatal("Value is not TINT");

		int result = value->value;
		Stack_Pop();
		return result;
	}

	UAPI int64 Stack_PeekLong() {
		TLONG* value = dynamic_cast<TLONG*>(arrStack.GetLast());
		if (!value)
			Message::Fatal("Value is not TLONG");

		int64 result = value->value;
		Stack_Pop();
		return result;
	}

	UAPI uint Stack_PeekUInt() {
		TUINT* value = dynamic_cast<TUINT*>(arrStack.GetLast());
		if (!value)
			Message::Fatal("Value is not TUINT");

		uint result = value->value;
		Stack_Pop();
		return result;
	}

	UAPI uint64 Stack_PeekULong() {
		TULONG* value = dynamic_cast<TULONG*>(arrStack.GetLast());
		if (!value)
			Message::Fatal("Value is not TULONG");

		uint64 result = value->value;
		Stack_Pop();
		return result;
	}

	UAPI float Stack_PeekFloat() {
		TFLOAT* value = dynamic_cast<TFLOAT*>(arrStack.GetLast());
		if (!value)
			Message::Fatal("Value is not TFLOAT");

		float result = value->value;
		Stack_Pop();
		return result;
	}

	UAPI double Stack_PeekDouble() {
		TDOUBLE* value = dynamic_cast<TDOUBLE*>(arrStack.GetLast());
		if (!value)
			Message::Fatal("Value is not TDOUBLE");

		double result = value->value;
		Stack_Pop();
		return result;
	}
}