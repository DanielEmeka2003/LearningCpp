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

// Rust
fn fibonacci() -> impl Iterator<Item = u64> {
	std::iter::successors(Some((0, 1)), |&(a, b)| Some((b, a + b))).map(|(a, _)| a) // Lazily extracts the Fibonacci number
}
// NcLang
fn fibonacci[type T]() T apply @T impls Iterator[ui8] {
	-> std:iter:successors(
		obj: maybe(obj: tuple(0, 1)),
		fn(obj val: tuple[ui8/a, ui8/b]) {
			-> obj: maybe(obj: tuple(val.b, val.a + val.b)).map(fn(obj val: tuple[ui8/a, ui8/_]) {-> val.a})
		}
	)
}


marco format[type (..T, UnicodeString, ResultUnicodeString)]('at:comptime obj format_unicode_string: UnicodeString, obj ..args: ..T) ResultUnicodeString
apply @UnicodeString impls unicode_string and @ResultUnicodeString impls unicode_string {
	'todo
}

unique matrix!(obj (row, column): ui) = arr[arr[ui4]!(column)]!(row)

b = at:move arrayList[3, 4, 5]