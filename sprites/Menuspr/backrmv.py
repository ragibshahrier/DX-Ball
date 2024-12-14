from rembg import remove
from PIL import Image
import os

def remove_background(input_image_path, output_image_path):
    try:
        # Open the input image
        with open(input_image_path, 'rb') as input_file:
            input_image = input_file.read()
        
        # Process the image to remove the background
        output_image = remove(input_image)
        
        # Save the result
        with open(output_image_path, 'wb') as output_file:
            output_file.write(output_image)
        
        print(f"Background removed successfully. Saved to {output_image_path}")
    except FileNotFoundError:
        print(f"Error: File '{input_image_path}' not found.")
    except Exception as e:
        print(f"Error: {e}")

# User input for file paths
input_path = input("Enter the path to the input image file: ").strip()

# Validate file existence
if not os.path.isfile(input_path):
    print("Error: The specified input file does not exist.")
else:
    # Output file path
    output_path = input("Enter the path to save the output image (e.g., output_image.png): ").strip()
    
    # Remove background
    remove_background(input_path, output_path)
