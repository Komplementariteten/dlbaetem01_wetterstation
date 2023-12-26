use std::io;
use std::io::{Read, Write};
use clap::Parser;

#[derive(Parser, Debug)]
#[command(author, version, about, long_about = None)]
struct Args{
    #[arg(short, long, required = true)]
    tty: String
}

fn main() {
    let args = Args::parse();

    println!("Opening Serial device {}!", args.tty);

    let mut port = match serialport::new(&args.tty, 115200).open() {
        Err(e) => {
            eprintln!("Failed to open \"{}\". Error: {:?}", &args.tty, e);
            ::std::process::exit(1);
        }
        Ok(p) => p
    };
    let mut serial_buf: Vec<u8> = vec![0; 1000];
    loop {
        match port.read(serial_buf.as_mut_slice()) {
            Ok(t) => io::stdout().write_all(&serial_buf[..t]).unwrap(),
            Err(ref e) if e.kind() == io::ErrorKind::TimedOut => (),
            Err(e) => eprintln!("{:?}", e),
        }
    }
    println!("Hello, world!");
}
