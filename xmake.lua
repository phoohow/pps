set_languages("c++20")

add_rules("mode.debug", "mode.release")
add_includedirs("ext/magic_enum/include")
includes("ext/sbin")

target("PPS", function()
    set_kind("shared")
    add_includedirs("include", {public = true})
    add_includedirs("src/include")
    
    add_deps("libsbin")
    add_files("src/*.cpp", "src/frontend/*.cpp", "src/pipeline/*.cpp")
    
    add_defines("PPS_EXPORT_DLL")
end)

target("TestLexer", function()
    set_kind("binary")
    add_includedirs("src/include")
    add_files("src/frontend/lexer.cpp", "src/test/testLexer.cpp")
end)

target("TestParser", function()
    set_kind("binary")
    add_includedirs("src/include")
    add_files("src/frontend/*.cpp")
    add_files("src/test/testParser.cpp")
end)

target("TestEvaluator", function()
    set_kind("binary")
    add_includedirs("include", "src/include")
    add_files("src/frontend/*.cpp")
    add_files("src/pipeline/*.cpp")
    add_files("src/test/testEvaluator.cpp")
end)

target("TestSimplifier", function()
    set_kind("binary")
    add_includedirs("include", "src/include")
    add_files("src/frontend/*.cpp")
    add_files("src/pipeline/*.cpp")
    add_files("src/test/testSimplifier.cpp")
end)

target("TestGenerator", function()
    set_kind("binary")
    add_includedirs("include", "src/include")
    add_files("src/frontend/*.cpp")
    add_files("src/pipeline/*.cpp")
    add_files("src/test/testGenerator.cpp")
end)

target("TestTaskBranch", function()
    set_kind("binary")
    add_deps("PPS")
    add_includedirs("src/include")
    add_files("src/test/testTaskBranch.cpp")
end)

target("TestTaskInclude", function()
    set_kind("binary")
    add_deps("PPS")
    
    local current_dir = os.scriptdir()
    -- transform the path to use forward slashes
    current_dir = current_dir:gsub("\\", "/")
    
    add_defines("SOURCE_DIR=\"" .. current_dir .. "\"")
    add_includedirs("src/include")
    add_files("src/test/testTaskInclude.cpp")
end)
