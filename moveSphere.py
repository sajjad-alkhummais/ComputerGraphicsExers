import sys

def read_obj_file(filename):
    """Read an OBJ file and return its contents as lines."""
    with open(filename, 'r') as f:
        return f.readlines()

def update_sphere_position(lines, new_x, new_y, new_z):
    """
    Update vertex positions in OBJ file.
    Translates all vertices to center the sphere at (new_x, new_y, new_z).
    """
    # First pass: find original center by averaging all vertices
    vertices = []
    for line in lines:
        if line.startswith('v '):
            parts = line.split()
            x, y, z = float(parts[1]), float(parts[2]), float(parts[3])
            vertices.append((x, y, z))
    
    if not vertices:
        print("No vertices found in file!")
        return lines
    
    # Calculate current center
    orig_x = sum(v[0] for v in vertices) / len(vertices)
    orig_y = sum(v[1] for v in vertices) / len(vertices)
    orig_z = sum(v[2] for v in vertices) / len(vertices)
    
    # Calculate translation needed
    dx = new_x - orig_x
    dy = new_y - orig_y
    dz = new_z - orig_z
    
    print(f"Original center: ({orig_x:.3f}, {orig_y:.3f}, {orig_z:.3f})")
    print(f"New center: ({new_x:.3f}, {new_y:.3f}, {new_z:.3f})")
    print(f"Translation: ({dx:.3f}, {dy:.3f}, {dz:.3f})")
    
    # Second pass: update vertices
    updated_lines = []
    for line in lines:
        if line.startswith('v '):
            parts = line.split()
            x = float(parts[1]) + dx
            y = float(parts[2]) + dy
            z = float(parts[3]) + dz
            updated_lines.append(f"v {x:.6f} {y:.6f} {z:.6f}\n")
        else:
            updated_lines.append(line)
    
    return updated_lines

def write_obj_file(filename, lines):
    """Write lines to an OBJ file."""
    with open(filename, 'w') as f:
        f.writelines(lines)

def main():
    # Configuration
    input_file = "ModelsFiles/sphere.obj"
    output_file = "ModelsFiles/sphere_updated.obj"
    
    # Desired position: in front of blue box, left of red box, on the ground
    # Floor is at y = -2.74
    # Sphere radius is approximately 1.0, so center y should be -2.74 + 1.0 = -1.74
    target_x = -1.3  # Between boxes, left of center
    target_y = -2.25  # On the ground
    target_z = 0.5    # In front of blue box
    
    # Allow command line arguments to override
    if len(sys.argv) >= 4:
        target_x = float(sys.argv[1])
        target_y = float(sys.argv[2])
        target_z = float(sys.argv[3])
    
    if len(sys.argv) >= 5:
        input_file = sys.argv[4]
    
    if len(sys.argv) >= 6:
        output_file = sys.argv[5]
    
    print(f"Reading from: {input_file}")
    print(f"Writing to: {output_file}")
    
    # Read, update, and write
    lines = read_obj_file(input_file)
    updated_lines = update_sphere_position(lines, target_x, target_y, target_z)
    write_obj_file(output_file, updated_lines)
    
    print(f"\nSuccessfully updated sphere position!")
    print(f"Output written to: {output_file}")

if __name__ == "__main__":
    main()