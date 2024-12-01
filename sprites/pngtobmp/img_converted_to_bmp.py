from PIL import Image
import os

def to_bmp(input_path, output_path=None, bg_color=(255, 255, 255)):
    with Image.open(input_path) as img:
        img = img.convert("RGBA")
        background = Image.new("RGB", img.size, bg_color)
        background.paste(img, mask=img.split()[3])
        img = background
        

        if output_path is None:
            output_path = os.path.splitext(input_path)[0] + ".bmp"
        
        img.save(output_path, format="BMP")
        print(f"Converted '{input_path}' to '{output_path}' successfully.")


print("Enter the folder path: ")
file_path = input()
print()

print("Enter the desired background RGB color (separated by a space): ")
r, g, b = input().split(" ")
bg_color = (int(r), int(g), int(b))
print(bg_color)
print()

count = 0
os.chdir(file_path)
for file in os.listdir():
    if (file[-4:] == ".png" or file[-4:] == ".jpg"):
        to_bmp(file,bg_color=bg_color)
        count += 1

print("\nAll Tasks Are Completed!")
print(f"Total Operation: {count}")



