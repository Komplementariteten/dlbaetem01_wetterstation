use hdf5::H5Type;

#[derive(H5Type, Copy, Clone, Debug, PartialEq)]
#[repr(C)]
pub(crate) struct Measurement {
    pub(crate) kind:u16,
    pub(crate) voltage:f32,
    pub(crate) adc: i32,
    pub(crate) count: u16
}

impl Measurement {
    pub(crate) fn new(c:u16, t:u16, adc:i32, f:f32) -> Self {
        Measurement {
            kind: t,
            adc: adc,
            count: c,
            voltage: f
        }
    }
}
