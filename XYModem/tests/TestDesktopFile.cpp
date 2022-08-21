#include "Files/DesktopFile.h"
#include "gtest/gtest.h"

namespace xymodem
{
class DesktopFileTest : public ::testing::Test
{
public:
    DesktopFileTest() : file (ghc::filesystem::current_path().append ("test.txt")) { file.create(); };
    File& getFile() { return file; }

    ~DesktopFileTest() { file.destroy(); }

private:
    DesktopFile file;
};

TEST_F (DesktopFileTest, TestExists)
{
    auto& file = getFile();
    EXPECT_TRUE (file.exists());
}

TEST_F (DesktopFileTest, TestDestroy)
{
    auto& file = getFile();
    file.destroy();
    EXPECT_FALSE (file.exists());
}

TEST_F (DesktopFileTest, TestCreation)
{
    auto& file = getFile();
    EXPECT_FALSE (file.create());
    EXPECT_TRUE (file.exists());
}

TEST_F (DesktopFileTest, TestOpen)
{
    auto& file = getFile();
    file.open();
    EXPECT_TRUE (file.isOpened());
}

TEST_F (DesktopFileTest, TestAppend)
{
    auto& file = getFile();
    file.open();
    file.append ({123, 125, 117});
    file.flush();
    EXPECT_EQ (file.getFilesize(), 3);
}

TEST_F (DesktopFileTest, TestRead)
{
    auto& file = getFile();
    file.open();
    file.append ({123, 125, 117});
    EXPECT_EQ (file.getNextFileBlock (3), "{}u");
}

TEST_F (DesktopFileTest, TestErase)
{
    auto& file = getFile();
    file.open();
    file.append ({123, 125, 117});
    EXPECT_EQ (file.getNextFileBlock (3), "{}u");
    file.erase();
    EXPECT_EQ (file.getFilesize(), 0);
}
} // namespace xymodem
