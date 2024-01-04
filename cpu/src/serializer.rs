use hdf5::{File, H5Type};
use chrono;
use chrono::{DateTime, Utc};
use ndarray::arr1;
use crate::contracts::Measurement;

const DATA_GROUP_NAME:&str = "measurements";
#[derive(Debug, Clone)]
pub(crate) struct Series {
    date: i64,
    data: Vec<Measurement>
}

pub(crate) fn serialize(file_name:&str, data: &Vec<Measurement>) -> Result<(),  Box<dyn std::error::Error>>{
    let series = Series{
        date: Utc::now().timestamp(),
        data: data.clone(),
    };
    let hdf5_file = File::append(file_name)?;
    let group = hdf5_file.group(DATA_GROUP_NAME).unwrap_or_else(|_| hdf5_file.create_group(DATA_GROUP_NAME).expect("can't create hdf5 group"));

    let builder = group.new_dataset_builder();
    let data_slice = series.data.as_slice();
    builder.with_data(&arr1(&data_slice)).create(series.date.to_string().as_str())?;
    Ok(())
}