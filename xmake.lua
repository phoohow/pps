set_languages("c++20")
add_rules("mode.debug", "mode.release")

add_includedirs("ext/magic_enum/include")
includes("ext/sbin")

option("enable_pps_test", function()
    set_default(false)
    set_showmenu(true)
    set_description("Enable unit tests for PPS")
end)

target("libpps", function()
    set_kind("shared")
    add_includedirs("include", {public = true})
    add_includedirs("src/include")
    
    add_deps("libsbin")
    add_deps("aclg")
    
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
        else
            add_deps("aclg")
        end
        
        for _, file in ipairs(files) do
            add_files(file)
        end
    end)
end

if has_config("enable_pps_test") then
    add_test_target("pps_lexer", false, {"src/frontend/lexer.cpp", "samples/pps_lexer.cpp"})
    add_test_target("pps_parser", false, {"src/frontend/*.cpp", "samples/pps_parser.cpp"})
    add_test_target("pps_evaluator", false, {"src/frontend/*.cpp", "src/pipeline/*.cpp", "samples/pps_evaluator.cpp"})
    add_test_target("pps_simplifier", false, {"src/frontend/*.cpp", "src/pipeline/*.cpp", "samples/pps_simplifier.cpp"})
    add_test_target("pps_generator", false, {"src/frontend/*.cpp", "src/pipeline/*.cpp", "samples/pps_generator.cpp"})
    add_test_target("pps_task_branch", true, {"samples/pps_task_branch.cpp"})
    add_test_target("pps_task_override", true, {"samples/pps_task_override.cpp"})
    
    target("pps_task_include", function()
        set_kind("binary")
        add_deps("libpps")
        
        local current_dir = os.scriptdir()
        -- transform the path to use forward slashes
        current_dir = current_dir:gsub("\\", "/")
        
        add_defines("SOURCE_DIR=\"" .. current_dir .. "\"")
        add_includedirs("src/include")
        add_files("samples/pps_task_include.cpp")
    end)
end
