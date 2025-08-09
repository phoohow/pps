set_languages("c++20")

add_rules("mode.debug", "mode.release")
add_includedirs("ext/magic_enum/include")
includes("ext/sbin")

option("test_pps", function()
    set_default(false)
    set_showmenu(true)
    set_description("Enable unit tests for PPS")
end)

target("libpps", function()
    set_kind("shared")
    add_includedirs("include", {public = true})
    add_includedirs("src/include")
    
    add_deps("libsbin")
    add_files("src/*.cpp", "src/frontend/*.cpp", "src/pipeline/*.cpp")
    
    add_defines("PPS_EXPORT_DLL")
end)

target("pps", function()
    set_kind("binary")
    add_deps("libpps")
    add_includedirs("include", "src/include")
    add_files("tools/pps.cpp")
end)

function add_test_target(name, need_pps, files)
    target(name, function()
        set_kind("binary")
        add_includedirs("include")
        add_includedirs("src/include")
        
        if need_pps then
            add_deps("libpps")
        end
        
        for _, file in ipairs(files) do
            add_files(file)
        end
    end)
end

if has_config("test_pps") then
    add_test_target("TestLexer", false, {"src/frontend/lexer.cpp", "test/testLexer.cpp"})
    add_test_target("TestParser", false, {"src/frontend/*.cpp", "test/testParser.cpp"})
    add_test_target("TestEvaluator", false, {"src/frontend/*.cpp", "src/pipeline/*.cpp", "test/testEvaluator.cpp"})
    add_test_target("TestSimplifier", false, {"src/frontend/*.cpp", "src/pipeline/*.cpp", "test/testSimplifier.cpp"})
    add_test_target("TestGenerator", false, {"src/frontend/*.cpp", "src/pipeline/*.cpp", "test/testGenerator.cpp"})
    add_test_target("TestTaskBranch", true, {"test/testTaskBranch.cpp"})
    add_test_target("TestTaskOverride", true, {"test/testTaskOverride.cpp"})
    
    target("TestTaskInclude", function()
        set_kind("binary")
        add_deps("libpps")
        
        local current_dir = os.scriptdir()
        -- transform the path to use forward slashes
        current_dir = current_dir:gsub("\\", "/")
        
        add_defines("SOURCE_DIR=\"" .. current_dir .. "\"")
        add_includedirs("src/include")
        add_files("test/testTaskInclude.cpp")
    end)
end
