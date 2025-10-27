
fn main() {
    let x = 0b11111111_u8 as u32;
    println!("{x}");
}

fn half_pi() -> f64 {
    let mut alternate_boolean = true;
    let mut numerator = 2_u32;
    let mut denominator = 1_u32;
    let mut half_pi_result = 1_f64;
    let mut temp = 0_f64;

    loop {
        temp = (numerator as f64)/(denominator as f64);

        if (temp - 1.0).abs() < 0.0000001 {
            break;
        }

        half_pi_result *= temp;

        if alternate_boolean {
            denominator += 2;
            alternate_boolean = false;
        }else {
            numerator += 2;
            alternate_boolean = true;
        }
    }

    half_pi_result
}