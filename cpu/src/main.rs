mod input_reader;
mod plotter;

use std::str;
use std::io::{Read, Write};
use std::process::Command;
use std::time::Duration;
use clap::Parser;
use crate::input_reader::{InputReader, ReadStatus};
use crate::plotter::plot;

#[derive(Parser, Debug)]
#[command(author, version, about, long_about = None)]
struct Args{
    #[arg(short, long, required = true)]
    tty: String
}




fn main() {
    let args = Args::parse();

    println!("Opening Serial device {}!", args.tty);

    let mut port = match serialport::new(&args.tty, 115200).timeout(Duration::from_secs(5 * 60)).open() {
        Err(e) => {
            eprintln!("Failed to open \"{}\". Error: {:?}", &args.tty, e);
            ::std::process::exit(1);
        }
        Ok(p) => p
    };
    let mut reader: InputReader = InputReader::new();
    let mut serial_buf: Vec<u8> = vec![0; 1000];
    loop {
        let readen = match port.read(serial_buf.as_mut_slice()) {
            Ok(t) => str::from_utf8(&serial_buf[..t]).unwrap().trim().to_string(),
            Err(e) => format!("{:?}", e),
        };
        match reader.read(readen.as_str()) {
            ReadStatus::Done => break,
            ReadStatus::Ok => ()
        }
    }
    reader.parse();
    let result1 = reader.adc_counts(1);
    let result2 = reader.adc_counts(2);
    let result3 = reader.adc_counts(3);
    plot(result1, result2, result3).unwrap();

    let mut out = Command::new("/usr/bin/xdg-open").arg("./plot.png").spawn().expect("can't open image");
    out.wait().expect("program not exited");
}
