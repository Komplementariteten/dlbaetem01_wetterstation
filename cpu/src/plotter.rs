use plotters::prelude::*;

const OUT_FILE_NAME:&str = "plot.png";
pub(crate) fn plot(data1:Vec<i32>, data2:Vec<i32>, data3:Vec<i32>)-> Result<(), Box<dyn std::error::Error>> {
    let root_area = BitMapBackend::new(OUT_FILE_NAME, (1024, 768)).into_drawing_area();
    root_area.fill(&WHITE);

    let root_area = root_area.titled("Temperature ADC Count values", ("sans-serif", 60))?;

    let mut cc = ChartBuilder::on(&root_area).margin(8).set_all_label_area_size(50).build_cartesian_2d(0.. data1.iter().count(), 0..0xFFFF)?;
    cc.configure_mesh().draw()?;
    cc.draw_series(LineSeries::new(data1.iter().enumerate().map(| (i, &y) | (i, y )), &RED)).unwrap().label("type1=red");
    cc.draw_series(LineSeries::new(data2.iter().enumerate().map(| (i, &y) | (i, y )), &BLUE)).unwrap().label("type2=blue");
    cc.draw_series(LineSeries::new(data3.iter().enumerate().map(| (i, &y) | (i, y )), &GREEN)).unwrap().label("type3=green");

    cc.configure_series_labels().background_style(&WHITE.mix(0.8)).border_style(&BLACK).draw()?;
    Ok(())
}