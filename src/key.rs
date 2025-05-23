use random_string::generate;

pub struct KeyGenerator(String);

impl KeyGenerator {
    fn new(numbers: bool, lowercase: bool, uppercase: bool) -> Self {
        if numbers == false && lowercase == false && uppercase == false {
            return Self(
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890".to_string(),
            );
        }
        let mut chars = String::new();
        if numbers {
            chars.push_str("1234567890");
        }
        if lowercase {
            chars.push_str("abcdefghijklmnopqrstuvwxyz");
        }
        if uppercase {
            chars.push_str("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        }
        return Self(chars);
    }

    fn generate(&mut self, length: usize) -> String {
        return generate(length, &self.0);
    }
}

/// Generates a random key
pub fn generate_key(size: usize, numbers: bool, lowercase: bool, uppercase: bool) -> String {
    return KeyGenerator::new(numbers, lowercase, uppercase).generate(size);
}
