param(
    [string]$Database = '',
    [string]$Mod = ''
)

# A small convenience script. The game itself is entirely C++ and SQL.
$ErrorActionPreference = 'Stop'
try {
    $cmakeCommand = Get-Command cmake -ErrorAction SilentlyContinue
    $cmakePath = if ($cmakeCommand) { $cmakeCommand.Source } else { $null }
    if (-not $cmakePath) {
        $vswherePath = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
        if (Test-Path -LiteralPath $vswherePath) {
            $installations = & $vswherePath -all -products '*' -property installationPath
            foreach ($installation in $installations) {
                $candidate = Join-Path $installation 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
                if (Test-Path -LiteralPath $candidate) {
                    $cmakePath = $candidate
                    break
                }
            }
        }
    }
    if (-not $cmakePath) {
        throw 'CMake was not found. Install Visual Studio Desktop development with C++, including CMake tools and the Windows SDK.'
    }

    $buildPath = Join-Path $PSScriptRoot 'build'
    & $cmakePath -S $PSScriptRoot -B $buildPath
    if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed. See the message above.' }
    & $cmakePath --build $buildPath --config Debug
    if ($LASTEXITCODE -ne 0) { throw 'Compilation failed. See the message above.' }

    $executable = Join-Path $buildPath 'Debug\dungeon_workshop.exe'
    if (-not (Test-Path -LiteralPath $executable)) {
        $executable = Join-Path $buildPath 'dungeon_workshop.exe'
    }
    $gameArguments = @()
    if ($Database) { $gameArguments += @('--db', $Database) }
    if ($Mod) { $gameArguments += @('--mod', $Mod) }
    & $executable @gameArguments
    exit $LASTEXITCODE
}
catch {
    Write-Host $_.Exception.Message -ForegroundColor Red
    exit 1
}
