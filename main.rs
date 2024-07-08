//use std::boxed::Box;

use std::time::{Duration, Instant};

// Logarithmic method
fn count_digits_logarithmic(n: u128, base: u32) -> u32 {
    if n == 0 {
        return 1;
    }
    (n as f64).log(base as f64).floor() as u32 + 1
}

// Iterative method
fn count_digits_iterative(mut n: u128, base: u32) -> u32 {
    if n == 0 {
        return 1;
    }
    let mut count = 0;
    while n > 0 {
        n /= base as u128;
        count += 1;
    }
    count
}

// Measure execution time
fn measure_time<F>(func: F, n: u128, base: u32, iterations: u32) -> Duration
where
    F: Fn(u128, u32) -> u32,
{
    let start = Instant::now();
    for _ in 0..iterations {
        func(n, base);
    }
    start.elapsed() / iterations
}

fn main() {
    let number = 89080812345678901234567890u128;
    let base = 10;
    let iterations = 100000;

    // Measure Logarithmic Method
    let log_time = measure_time(count_digits_logarithmic, number, base, iterations);
    println!("Logarithmic Method: Average Time: {:?}", log_time);

    // Measure Iterative Method
    let iter_time = measure_time(count_digits_iterative, number, base, iterations);
    println!("Iterative Method: Average Time: {:?}", iter_time);
}

// fn match(){  }

/*
Emeka
Daniel
Fly
is
Flying
To
The Moon
Baby
*/