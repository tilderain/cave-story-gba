#!/bin/bash

# Ensure the destination directory exists
mkdir -p ../maxmod_data

# Loop through all .org files in the current directory
for file in *.org; do
    # Check if file actually exists
    [ -e "$file" ] || continue
    
    # Get the filename without the extension
    base="${file%.org}"
    
    echo "Processing $base..."
    
    # --- Pass 1: English Data ---
    echo "  Generating EN version..."
    ./org2 "$file" ORG210EN.DAT
    
    if [ -f "$base.xm" ]; then
        cp "$base.xm" ../maxmod_data/
    else
        echo "  Warning: $base.xm not generated for EN pass."
    fi

    # --- Pass 2: Japanese Data ---
    echo "  Generating JP version..."
    ./org2 "$file" ORG210JP.DAT
    
    # Copy the file with the D_ prefix
    # (Assuming org2 prefixes the output or you are looking for that specific file)
    if [ -f "D_$base.xm" ]; then
        cp "D_$base.xm" ../maxmod_data/
    elif [ -f "$base.xm" ]; then
        # Fallback in case the tool doesn't prefix automatically but you need it renamed
        cp "$base.xm" "../maxmod_data/D_$base.xm"
    else
        echo "  Warning: D_$base.xm not found for JP pass."
    fi
done

echo "Done! Files copied to ../maxmod_data"