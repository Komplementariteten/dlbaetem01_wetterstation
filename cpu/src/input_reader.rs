use crate::contracts::Measurement;
use crate::serializer::serialize;

pub(crate) struct InputReader {
    raw:String,
    data:Vec<Measurement>
}

pub(crate) enum ReadStatus {
    Ok,
    Done
}

impl InputReader {

    pub(crate) fn new() -> Self {
        return InputReader {
            data: vec![],
            raw: "".to_string()
        }
    }
    pub(crate) fn read(&mut self, line:&str) -> ReadStatus {
        if line.trim().starts_with("DONE") {
            return ReadStatus::Done
        }
        self.raw.push_str(line.trim());

        return ReadStatus::Ok
    }

    pub(crate) fn print(&self) {
        println!("{}", self.raw);
    }

    pub(crate) fn parse(&mut self) {
        for line in self.raw.as_str().split('#') {
            let mut parts = line.split(';');
            if parts.clone().count() >= 4 {
                let c = parts.next().unwrap().trim().parse::<u16>().unwrap();
                let t = parts.next().unwrap().trim().parse::<u16>().unwrap();
                let adc_str = parts.next().unwrap().trim();
                let adc = adc_str.parse::<i32>().unwrap();
                let f = parts.next().unwrap().trim().parse::<f32>().unwrap();
                self.data.push(Measurement::new(c, t, adc, f));
            }
        }
        self.raw = "".to_string();
    }
    pub(crate) fn adc_counts(&self, kind:u16) -> Vec<i32> {
        self.data.iter().filter(| &p | p.kind == kind).map(|&d | d.adc).collect()
    }

    pub(crate) fn serialize(&self) {
        serialize("d.h5", &self.data).expect("TODO: panic message");
    }

}