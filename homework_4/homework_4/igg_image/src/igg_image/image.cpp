#include "image.h"

namespace igg {

  // Downscaling function definition
  void Image::DownScale(int scale) {

    // Create the downscaled image
    int downscaled_rows = rows_/scale;
    int downscaled_cols = cols_/scale;
    std::vector<Pixel> downscaled_image;
    downscaled_image.reserve(downscaled_rows * downscaled_cols);

    // Iterate over image
    for (int i = 0; i < rows_; i+=scale) {
        for (int j = 0; j < cols_; j+=scale) {
            downscaled_image.emplace_back(data_[i * cols_ + j]);
        }
    }

    rows_ = downscaled_rows;
    cols_ = downscaled_cols;
    data_ = downscaled_image;

  }

  // Upscaling function definition
  void Image::UpScale(int scale) {

    // Create upscaled image
    int upscaled_rows = rows_ * scale;
    int upscaled_cols = cols_ * scale;
    std::vector<Pixel> upscaled_image(upscaled_rows * upscaled_cols,
                                      {0, 0, 0});

    // Iterate over the current image
    for (int i = 0; i < rows_; i++) {
        int row_jump = i * scale;
        for (int j = 0; j < cols_; j++) {
            int col_jump = j*scale;
            // Fill values for upscaled image
            for (int u = 0; u < scale; u++) {
                for (int v = 0; v < scale; v++) {
                    upscaled_image[(row_jump+u)*upscaled_cols + col_jump+v] 
                      = data_[i * cols_ + j];
                }
            }
        }
    }

    rows_ = upscaled_rows;
    cols_ = upscaled_cols;
    data_ = upscaled_image;

  }

  // Read Image from disk
  bool Image::ReadFromDisk(const std::string& file_name) {
    // Fail if strategy is not set
    if (!strategy_) {
      return false;
    }

    // Apply the strategy's Read method
    const ImageData& image = strategy_->Read(file_name);

    // If file does not exist or Dummy strategy is being used
    if (image.data.empty()) {
      return false;
    }

    // Fill the values for the class
    rows_ = image.rows;
    cols_ = image.cols;
    max_val_ = image.max_val;
    for (int i = 0, size = image.data[0].size(); i < size; i++) {
      Pixel p{image.data[0][i], image.data[1][i], image.data[2][i]};
      data_.emplace_back(p);
    }

    return true;
  }

  // Write image to disk
  bool Image::WriteToDisk(const std::string& file_name) {
    if (!strategy_) {
      return false;
    }

    ImageData image;
    image.rows = rows_;
    image.cols = cols_;
    image.max_val = max_val_;

    std::vector<int> red(rows_ * cols_, 0);
    std::vector<int> green(rows_ * cols_, 0);
    std::vector<int> blue(rows_ * cols_, 0);
    for (int idx = 0, size = data_.size(); idx < size; idx++) {
      red[idx] = data_[idx].red;
      green[idx] = data_[idx].green;
      blue[idx] = data_[idx].blue;
    }
    image.data = {red, green, blue};

    strategy_->Write(file_name, image);
    std::cout << "File: " << file_name << " written successfully."
              << std::endl;

    return true;
  }  

} // namespace igg