pub mod key;

use clap::Parser;
use colored::{ColoredString, Colorize};
use copypasta::ClipboardProvider;
use core::fmt;
use key::generate_key;
use std::path::PathBuf;

use prompt_input::prelude::*;
use std::fs::write;

use clap::builder::styling::AnsiColor;

// Took me a long time to figure this out ;-;

pub fn get_style() -> clap::builder::Styles {
    clap::builder::Styles::styled()
        .header(AnsiColor::Yellow.on_default())
        .usage(AnsiColor::Yellow.on_default())
        .literal(AnsiColor::BrightGreen.on_default())
        .placeholder(AnsiColor::BrightCyan.on_default())
        .valid(AnsiColor::BrightCyan.on_default())
}

/// Simple program to greet a person
#[derive(Parser, Debug, Clone)]
#[command(
    version,
    about,
    long_about = "Generates a Key that can be utlized for tools like Cryptomater or Veracrypt",
    styles = get_style()
)]
struct Args {
    /// Enables Numeric Letters of 0-9
    #[arg(short, long, default_value_t = false)]
    numeric: bool,

    /// Enables Lowercase letters in key
    #[arg(short, long, default_value_t = false)]
    lowercase: bool,

    /// Enables uppercase letters in key
    #[arg(short, long, default_value_t = false)]
    uppercase: bool,


    /// Number of characters to generate for your key
    /// Normally going over 20 characters on a file is 
    /// strong. 10 or more if your making a password
    #[arg(short, long, default_value_t = 20)]
    size: usize,

    /// Stores the key onto a save file,
    /// if your about to override a file
    /// this tool will try attempt to warn you
    /// and ask for confirmation.
    /// You can disable that using -f or --force
    #[arg(short, long)]
    output: Option<PathBuf>,

    /// Forces Key to overwrite given file without confirmation
    #[arg(short, long, default_value_t = false)]
    force: bool,

    /// Disables terminal Colors (Mainly Warnings)
    #[arg(short, long, default_value_t = false)]
    disable_color: bool,

    /// Copies your newly generated key to your 
    /// clipboard
    #[arg(short, long, default_value_t = false)]
    clip_board:bool

}

impl Args {
    pub fn label<S: fmt::Display>(
        &self,
        label: &'static str,
        text: S,
        background: Option<colored::Color>,
        foreground: Option<colored::Color>,
    ) {
        match self.disable_color {
            true => {
                println!("{label}: {text}");
            }
            false => {
                let mut label_text = ColoredString::from(label).bright_white();
                label_text.bgcolor = background;
                label_text.fgcolor = foreground;
                println!("{}: {}", label_text, text);
            }
        }
    }

    /// Warns user about possible problem
    #[inline]
    pub fn warn<S: fmt::Display>(&self, text: S) {
        self.label("[WARN]", text, Some(colored::Color::Red), None);
    }

    pub fn prompt<S: fmt::Display>(
        &self,
        label: &'static str,
        text: S,
        background: colored::Color,
    ) -> String {
        match self.disable_color {
            true => String::prompt(format_args!("{label}: {text}").as_str().unwrap()),
            false => {
                let mut label_text = ColoredString::from(label).bright_white();
                label_text.bgcolor.replace(background);
                String::prompt(format_args!("{label}: {text}").as_str().unwrap())
            }
        }
    }

    /// Confirmation of changes to something...
    #[inline]
    pub fn confirmation<S: fmt::Display>(&self, text: S) -> bool {
        self.prompt("[Y/N]", text, colored::Color::BrightYellow)
            .to_lowercase()
            == "y"
    }

    #[inline]
    pub fn sucess<S: fmt::Display>(&self, label: &'static str, text: S) {
        self.label(label, text, None, Some(colored::Color::Green));
    }

    pub fn generate_key(&self) {
        if self.size == 0 {
            self.label("[ERROR]", "Keys can't have 0 characters", None, Some(colored::Color::Red));
        }
        
        if self.size <= 8 {
            self.warn("The Key you're about to generate is exteremely weak and brute-forcers will be able to recover it quickly.");
        }
        
        let key = generate_key(self.size, self.numeric, self.lowercase, self.uppercase);
        if self.output.is_some() {
            // Warn if were about overwrite important files or previous keys
            if self.output.as_ref().is_some_and(|x| x.exists()) && !self.force {
                self.warn(
                    format_args!(
                        "Your about to overwrite {}",
                        self.output.as_ref().unwrap().to_string_lossy()
                    )
                    .as_str()
                    .unwrap(),
                );
                if !self.confirmation("Would you like to overwrite it:") {
                    return;
                };
            }

            let path = self.output.as_ref().unwrap();
           
            match write(
                path,
                key) {
                Ok(_) => {
                    self.sucess(
                        "[Key Saved]",
                        format_args!("Your Key Was Saved as {}", path.to_string_lossy()),
                    );
                }
                Err(e) => {
                    self.label(
                        "[ERROR]",
                        e.to_string(),
                        Some(colored::Color::BrightRed),
                        None,
                    );
                }
            }
            return;
        } else {
            self.sucess(
                "[Key]:",
                format_args!(
                    "Your Key is \"{}\" Copy it or write it down.",
                    key
                ),
            );
        }
        if self.clip_board {
            copypasta::ClipboardContext::new().unwrap().set_contents(key).unwrap();
            self.sucess("[Copied]", "Your Key was copied to your clipboard");
        }
    }
}

fn main() {
    if cfg!(windows){
        let mut args = Args::parse();
    
        #[cfg(windows)]
        match ansi_term::enable_ansi_support() {
            Ok(_) => {}
            Err(_) => {
                println!("[WARN] Colors will be disabled, is this what you wanted?");
                args.disable_color = true;
            }
        };
        args.generate_key();
    } else {
        Args::parse().generate_key();
    }
}
