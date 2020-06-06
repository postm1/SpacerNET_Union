// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	struct TSTACKTYPE {
		virtual void Destroy() = 0;
		virtual string GetTypeName() = 0;
		virtual ~TSTACKTYPE() { }
	};

	struct TSTRING : public TSTACKTYPE {
		virtual void Destroy() { delete this; }
		virtual string GetTypeName() { return "TSTRING"; }
		string value;
	};

	struct TSTRINGW : public TSTACKTYPE {
		virtual void Destroy() { delete this; }
		virtual string GetTypeName() { return "TSTRINGWIDE"; }
		CStringW value;
	};


	struct TINT : public TSTACKTYPE {
		virtual void Destroy() { delete this; }
		virtual string GetTypeName() { return "TINT"; }
		int value;
	};

	struct TLONG : public TSTACKTYPE {
		virtual void Destroy() { delete this; }
		virtual string GetTypeName() { return "TLONG"; }
		int64 value;
	};

	struct TUINT : public TSTACKTYPE {
		virtual void Destroy() { delete this; }
		virtual string GetTypeName() { return "TUINT"; }
		uint value;
	};

	struct TULONG : public TSTACKTYPE {
		virtual void Destroy() { delete this; }
		virtual string GetTypeName() { return "TULONG"; }
		uint64 value;
	};

	struct TFLOAT : public TSTACKTYPE {
		virtual void Destroy() { delete this; }
		virtual string GetTypeName() { return "TFLOAT"; }
		float value;
	};

	struct TDOUBLE : public TSTACKTYPE {
		virtual void Destroy() { delete this; }
		virtual string GetTypeName() { return "TDOUBLE"; }
		double value;
	};
}