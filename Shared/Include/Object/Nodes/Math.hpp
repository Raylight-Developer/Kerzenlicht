#pragma once

#include "Shared.hpp"

#include "Core/Nodes.hpp"

// FWD DECL OTHER

// FWD DECL THIS
namespace KL::NODE::MATH {
	enum struct Type;
	struct Arithmetic;
	namespace ARITHMETIC {
		enum struct Type;
		struct Multiplication;
		struct Subtraction;
		struct Addition;
		struct Division;
		struct Power;
	}
	struct Interpolation;
	namespace INTERPOLATION {
		enum struct Type;
		struct Linear;
		struct Ease;
		struct Ease_Overshoot;
	}
}

// DECL
namespace KL::NODE::MATH {
	enum struct Type {
		NONE,
		ARITHMETIC,
		INTERPOLATION
	};
	struct Arithmetic : Node {
		ARITHMETIC::Type mini_type;

		PORT::Data_I_Port* i_a;
		PORT::Data_I_Port* i_b;
		PORT::Data_O_Port* o_res;

		Arithmetic();
	};
	namespace ARITHMETIC {
		enum struct Type {
			NONE,
			MULTIPLICATION,
			SUBTRACTION,
			ADDITION,
			DIVISION,
			POWER
		};
		struct Multiplication : Arithmetic {
			Multiplication();
			Prop getData(const uint16& slot_id) const override;
		};
		struct Subtraction : Arithmetic {
			Subtraction();
			Prop getData(const uint16& slot_id) const override;
		};
		struct Addition : Arithmetic {
			Addition();
			Prop getData(const uint16& slot_id) const override;
		};
		struct Division : Arithmetic {
			Division();
			Prop getData(const uint16& slot_id) const override;
		};
		struct Power : Arithmetic {
			Power();
			Prop getData(const uint16& slot_id) const override;
		};
	}
	namespace INTERPOLATION {
	}
}