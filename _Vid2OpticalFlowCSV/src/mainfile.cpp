#include "stdafx.h"


#include "lk/LK2CSV.h"
#include "util/tclap/CmdLine.h"

int main(int argc, char* argv[])
{
	TCLAP::CmdLine cmd("Command description message", ' ', "1.0");

	TCLAP::ValueArg<std::string> video_input("v", "video", "Input Video File", true, "", "string"); cmd.add(video_input);
	TCLAP::ValueArg<std::string> out_csv("o", "out", "Output CSV File", false, "lk-out.csv", "string"); cmd.add(out_csv);
	TCLAP::ValueArg<std::string> config_file("c", "config", "Config File", false, "default-config.xml", "string"); cmd.add(config_file);
	TCLAP::ValueArg<std::string> dump_dir("d", "dump", "Dump Directory", false, "", "string"); cmd.add(dump_dir);
	TCLAP::ValueArg<std::string> lens_correction("l", "lens", "Lens Correction Param File", false, "", "string"); cmd.add(lens_correction);
	cmd.parse(argc, argv);


	LK2CSV::GenerateLKData(video_input.getValue(), out_csv.getValue(),  config_file.getValue(), dump_dir.getValue(), lens_correction.getValue());


	return 0;
}


