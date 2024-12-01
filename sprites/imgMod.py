import sys
from PIL import Image
import os

def lower_resolution(input_path, output_path, new_width, new_height, keep_aspect_ratio=True, max_file_size=None):
    """
    Reduces the resolution of an image to the specified dimensions and limits file size if required.

    Args:
        input_path (str): Path to the input image.
        output_path (str): Path to save the output image.
        new_width (int): The desired width of the output image.
        new_height (int): The desired height of the output image.
        keep_aspect_ratio (bool): Whether to preserve the original aspect ratio.
        max_file_size (int): Maximum allowed file size in kilobytes (KB).
    """
    try:
        # Open the image file
        with Image.open(input_path) as img:
            if keep_aspect_ratio:
                # Calculate aspect ratio and adjust dimensions
                original_width, original_height = img.size
                aspect_ratio = original_width / original_height

                if new_width / new_height > aspect_ratio:
                    # Adjust width to maintain aspect ratio
                    new_width = int(new_height * aspect_ratio)
                else:
                    # Adjust height to maintain aspect ratio
                    new_height = int(new_width / aspect_ratio)

            # Resize the image to the target dimensions
            img_resized = img.resize((new_width, new_height), Image.Resampling.LANCZOS)

            # Save the resized image with optional file size limit
            output_format = "JPEG"
            output_path = os.path.splitext(output_path)[0] + ".jpg"  # Force JPEG format

            if max_file_size:
                quality = 95  # Start with high quality

                # Iteratively adjust quality to meet file size
                while True:
                    img_resized.save(output_path, format=output_format, optimize=True, quality=quality)
                    file_size_kb = os.path.getsize(output_path) / 1024  # File size in KB

                    if file_size_kb <= max_file_size or quality <= 10:
                        # Stop if the file size meets the requirement or quality is too low
                        break

                    # Decrease quality to reduce file size
                    quality -= 5

                if file_size_kb > max_file_size:
                    print(f"Warning: Could not reduce file size below {max_file_size} KB.")
            else:
                # Save without size restriction
                img_resized.save(output_path, format=output_format, optimize=True)

            print(f"Image saved to {output_path} with resolution {new_width}x{new_height} and size {os.path.getsize(output_path) / 1024:.2f} KB.")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 5:
        print("Usage: python lower_resolution.py <input_path> <output_path> <width> <height> [preserve_aspect_ratio] [max_file_size]")
        sys.exit(1)

    input_path = sys.argv[1]
    output_path = sys.argv[2]
    new_width = int(sys.argv[3])
    new_height = int(sys.argv[4])

    # Optional arguments
    keep_aspect_ratio = sys.argv[5].lower() == "true" if len(sys.argv) > 5 else True
    max_file_size = int(sys.argv[6]) if len(sys.argv) > 6 else None

    lower_resolution(input_path, output_path, new_width, new_height, keep_aspect_ratio, max_file_size)
