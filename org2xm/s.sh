#!/bin/bash

# Loop through all .org files in the current directory
for file in *.org; do
    # Check if file actually exists
    [ -e "$file" ] || continue
    
    # Get the filename without the extension (e.g., "music.org" -> "music")
    base="${file%.org}"
    
    echo "Processing $base..."
    
    # Run the org2 command
    ./org2 "$file" ORG210EN.DAT
    
    # Run the xm2it command using the base name
    # This assumes a file named "basename.xm" exists
    if [ -f "$base.xm" ]; then
        ./xm2it "$base.xm" "$base.it"
    else
        echo "Warning: $base.xm not found, skipping xm2it conversion."
    fi
done

  
