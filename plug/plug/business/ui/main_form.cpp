#include "../../business/ui/main_form.h"

#include <stdlib.h> 

#include "../../third_party/nana/include/nana/gui/programming_interface.hpp"
#include "../../third_party/nana/include/nana/gui/basis.hpp"
#include "../../skill/privilege/promote_privilege.h"
#include "../../business/algorithm/link_game_type.h"
#include "../../business/algorithm/link_game.h"

#include <nana/gui/widgets/button.hpp>
#include <nana/paint/image.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/programming_interface.hpp>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

namespace
{
const wchar_t* game = L"QQ游戏 - 连连看角色版";
const int clearSecode = 1500;
const int startSecode = 10 * 1000;
}

MainForm* MainForm::main_form_ = nullptr;

MainForm::~MainForm()
{
    if (main_form_)
    {
        main_form_ = nullptr;
    }
}

MainForm* MainForm::Get()
{
    if (!main_form_)
        main_form_ = new MainForm();

    return main_form_;
}

MainForm::MainForm()
    : nana::form(nana::API::make_center(310, 130),
    nana::appearance(true, true, false, true, true, false, true))
    , start_()
    , gameHwnd_()
    , current_pt_()
    , link_game_()
    , icon_()
    , timer_()
    , auto_wait_for_()
    , auto_start_()
    , start_timer_()
    , all_clear_()
    , slider_()
    , slider_switch_()
{
    HWND hWnd = reinterpret_cast<HWND>(nana::API::root(this->handle()));
    this->events().destroy.connect(std::bind(&MainForm::OnDestory, this, _1));
    LONG style = ::GetWindowLong(hWnd, GWL_STYLE);
    ::SetWindowLong(hWnd, GWL_STYLE, style & ~WS_THICKFRAME);
    start_.reset(new nana::button(*this, nana::rectangle(123, 20, 50, 30)));
    start_->events().click.connect(std::bind(&MainForm::OnStartClick, this, _1));
    start_->caption(L"开始");

    icon_.reset(new nana::paint::image(L""));
    icon(*icon_.get());

    timer_.reset(new nana::timer());
    timer_->elapse(std::bind(&MainForm::OnTimer, this));

    auto_wait_for_.reset(new nana::checkbox(*this, nana::rectangle(20, 16, 80, 20)));
    auto_wait_for_->check(false);
    auto_wait_for_->caption(L"自动挂机");
    auto_wait_for_->transparent(false);
    auto_wait_for_->events().click.connect(std::bind(&MainForm::OnAutoWaitFor, this, _1));

    auto_start_.reset(new nana::checkbox(*this, nana::rectangle(160, 16, 80, 20)));
    auto_start_->check(false);
    auto_start_->caption(L"自动开局");
    auto_start_->transparent(false);
    auto_start_->events().click.connect(std::bind(&MainForm::OnAutoStart, this, _1));
    //bgcolor(nana::color);

    start_timer_.reset(new nana::timer());
    start_timer_->elapse(std::bind(&MainForm::OnStartTimer, this));

    all_clear_.reset(new nana::button(*this, nana::rectangle(255, 69, 40, 30)));
    all_clear_->caption(L"秒杀");
    all_clear_->events().click.connect(std::bind(&MainForm::OnAllClear, this, _1));

    slider_switch_.reset(new nana::checkbox(*this, nana::rectangle(20, 50, 80, 20)));
    slider_switch_->check(false);
    slider_switch_->caption(L"启动调节器");
    slider_switch_->transparent(false);
    slider_switch_->events().click.connect(std::bind(&MainForm::OnEnableSlider, this, _1));

    slider_.reset(new nana::slider(*this, true));
    slider_->size(nana::size(150, 20));
    slider_->vmax(1500);
    slider_->move(20, 76);
    slider_->events().value_changed.connect(std::bind(&MainForm::OnSliderChangedValue, this, _1));
    slider_->events().mouse_move.connect(std::bind(&MainForm::OnSliderMouseMove, this, _1));
    slider_->enabled(false);

    moust_top_.reset(new nana::checkbox(*this, nana::rectangle(120, 70, 80, 20)));
    moust_top_->caption(L"总是最前");
    moust_top_->check(false);
    moust_top_->events().click.connect(std::bind(&MainForm::OnMoustTop, this, _1));
}

void MainForm::RunApp()
{
    if (main_form_)
    {
        main_form_->show();
        wait_for_this();
    }
}

void MainForm::OnStartClick(const nana::arg_click& arg)
{    
    if (OnClickStart())
    {
        if (!link_game_)
            link_game_.reset(new plug::LinkGameEraser(
            std::bind(&MainForm::ClickTwoPoint, this, _1, _2)));        
    }
    else
    {
        ::MessageBox(NULL, L"游戏没有运行!", L"WARING", MB_ICONWARNING | MB_OK);
    }
}

void MainForm::OnDestory(const nana::arg_destroy& arg)
{
}

void MainForm::OnReadMemoryData()
{
    DWORD processId = 0;
    ::GetWindowThreadProcessId(gameHwnd_, &processId);
    HANDLE handle = plug::OpenProcess(processId);
    if (!handle)
    {
        plug::PromotePrivilege();
        handle = plug::OpenProcess(processId);
    }
    BYTE buffer[11][19] = { 0 };
    void* baseValue = reinterpret_cast<void*>(0x00189F78);
    SIZE_T sz = 0;

    ::ReadProcessMemory(handle, baseValue, reinterpret_cast<void*>(&buffer),
                        11 * 19, &sz);

    if (link_game_)
        link_game_->SetBoardData(buffer);
}

bool MainForm::ClickTwoPoint(const POINT first, const POINT secode)
{
    if (!gameHwnd_)
    {
        gameHwnd_ = ::FindWindow(NULL, game);
    }

    int lParam = ((first.y * 35 + 192) << 16) + (first.x * 31 + 21);
    ::SendMessage(gameHwnd_, WM_LBUTTONDOWN, 0, lParam);
    ::SendMessage(gameHwnd_, WM_LBUTTONUP, 0, lParam);

    lParam = ((secode.y * 35 + 192) << 16) + (secode.x * 31 + 21);
    ::SendMessage(gameHwnd_, WM_LBUTTONDOWN, 0, lParam);
    ::SendMessage(gameHwnd_, WM_LBUTTONUP, 0, lParam);
    
    return true;
}

void MainForm::OnTimer()
{
    if (link_game_)
    {
        link_game_->ClearPair();
    }
}

void MainForm::OnAutoWaitFor(const nana::arg_click& arg)
{
    if (!timer_ || !auto_wait_for_)
        return;

    if (auto_wait_for_->checked())
    {
        if (!timer_->started())
        {
            timer_->interval(clearSecode);
            timer_->start();
        }
    }
    else
    {
        if (timer_->started())
        {
            timer_->stop();
        }
    }
}

void MainForm::OnAutoStart(const nana::arg_click& arg)
{
    if (!auto_start_ || !start_timer_)
        return;

    if (auto_start_->checked())
    {
        if (!start_timer_->started())
        {
            start_timer_->interval(startSecode);
            start_timer_->start();
        }
    }
    else
    {
        if (start_timer_->started())
            start_timer_->stop();
    }
}

void MainForm::OnStartTimer()
{
    OnClickStart();
}

bool MainForm::OnClickStart()
{
    gameHwnd_ = ::FindWindow(NULL, game);
    if (gameHwnd_)
    {
        RECT  rc = { 0 };
        ::GetWindowRect(gameHwnd_, &rc);
        ::GetCursorPos(&current_pt_);
        POINT pt = { rc.left + 663, rc.top + 564 };
        ::SetCursorPos(pt.x, pt.y);
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
        Sleep(50);
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);
        ::SetCursorPos(current_pt_.x, current_pt_.y);
        return true;
    }

    return false;
}

void MainForm::OnAllClear(const nana::arg_click& arg)
{
    OnReadMemoryData();
}

void MainForm::OnEnableSlider(const nana::arg_click& arg)
{
    if (!slider_ || !slider_switch_)
        return;
    
    bool checked = slider_switch_->checked();
    slider_->enabled(checked);
}

void MainForm::OnSliderChangedValue(const nana::arg_slider& arg)
{
    if (!auto_wait_for_ || !timer_)
        return;

    if (auto_wait_for_->checked() && !timer_->started())
    {
        timer_->interval(arg.widget.value() * 10);
    }
}

void MainForm::OnSliderMouseMove(const nana::arg_mouse& arg)
{
    if (!slider_)
        return;

    std::wostringstream os;
    os << arg.pos.x + 1;
    slider_->tooltip(os.str());
}

void MainForm::OnMoustTop(const nana::arg_click& arg)
{
    if (!moust_top_)
        return;

    HWND hWnd = ::FindWindow(NULL, game);    
    if (moust_top_->checked())
    {
        moust_top_->caption(L"取消游戏最前");
        ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, 
                       SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }
    else
    {   
        moust_top_->caption(L"游戏总是最前");
        ::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 
                       SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }
}
