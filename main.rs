use std::result::Result;

fn main() {
	'block: {
		let mut vec = vec![1, 2, 3];
		{ // First sequential borrow
			let first = &mut vec[0];
			*first += 3;
			println!("Inside block: first={first}");
		}
		{ // Second sequential borrow
			let second = &mut vec[1];
			*second += 9;
			println!("Inside block: second={second}");
		}
		let third = &mut vec[2];
		let stolen_vec = vec;
		println!("{third} {stolen_vec:?}");

		break 'block;
		// print!("{:?}", vec);
	}

	println!("Outside block");

	let fib = fibonacci().take(10); // Takes only the first 10 values
	for num in fib {
		println!("Fibonacci {}", num);
	}
}

fn fibonacci() -> impl Iterator<Item = u64> {
	std::iter::successors(Some((0, 1)), |&(a, b)| Some((b, a + b))).map(|(a, _)| a) // Lazily extracts the Fibonacci number
}

marco format[type (..T, UnicodeString, ResultUnicodeString)]('at:comptime obj format_unicode_string: UnicodeString, obj ..args: ..T) ResultUnicodeString
apply @UnicodeString impls unicode_string and @ResultUnicodeString impls unicode_string {
	'todo
}

/*
	Default Constructor
	class House {
		House() {}
	}
*/

// struct g apply true = ... (problem remember)

// Grouping semantics for type parameters
// [type (T, Value)]

// Access restriction combinations
// 'at:hide(type, mod) => The order matters `types in the mod`
// 'at:hide(type, pkg)
//
// 'at:readonly(mod)