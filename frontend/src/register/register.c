#include <gtk/gtk.h>
#include <regex.h>
#include <string.h>
#include <arpa/inet.h>
#include "register.h"
#include "../auth/auth.h"
#include "../login/login.h"
#include "../global/global.h"

GtkWidget *entry_confirm_password;
GtkWidget *entry_username;
GtkWidget *entry_phone;

void on_register(GtkWidget *widget, gpointer data) {
    const char *email = gtk_entry_get_text(GTK_ENTRY(entry_email));
    const char *password = gtk_entry_get_text(GTK_ENTRY(entry_password));
    const char *confirm_password = gtk_entry_get_text(GTK_ENTRY(entry_confirm_password));
    const char *username = gtk_entry_get_text(GTK_ENTRY(entry_username));
    const char *phone = gtk_entry_get_text(GTK_ENTRY(entry_phone));

    if (strlen(email) == 0 || strlen(password) == 0 || strlen(confirm_password) == 0 || 
        strlen(username) == 0 || strlen(phone) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "All fields are required!");
        return;
    }

      if (!validate_phone(phone)){
        gtk_label_set_text(GTK_LABEL(label_status), "Phone must be in number, 10-15 numbers");
        return;
    }

    if (!validate_email(email)) {
        gtk_label_set_text(GTK_LABEL(label_status), "Email is invalid!");
        return;
    }

    if (!validate_password(password)) {
        gtk_label_set_text(GTK_LABEL(label_status), "Password must be at least 8 digits, uppercase and number");
        return;
    }

    if (strcmp(password, confirm_password) != 0){
        gtk_label_set_text(GTK_LABEL(label_status), "Confirm password is unmatch");
        return;
    }
    
    snprintf(buffer, MAX_LENGTH, "REGISTER %s:%s:%s:%s", username, phone, email, password);
    send(sock, buffer, sizeof(buffer), 0);
    g_print("Sent to server: %s\n", buffer);
    recv(sock, buffer, sizeof(buffer), 0);
    g_print("Received from server: %s\n", buffer);
    if (strcmp(buffer, "SUCCESS") == 0){
        GtkWidget *login_content = create_login_window();
        set_content(login_content);
    }
    else if (strcmp(buffer, "EMAIL_EXISTED") == 0){
        gtk_label_set_text(GTK_LABEL(label_status), "Email exists");
        return;
    }
    else {
        gtk_label_set_text(GTK_LABEL(label_status), "Register failed, please try again");
        return;
    }

}

void on_login_link_click (GtkWidget *widget, gpointer data){
    GtkWidget *login_content = create_login_window();
    set_content(login_content);
}

GtkWidget* create_register_window() {

     GtkCssProvider *provider = gtk_css_provider_new();
gtk_css_provider_load_from_data(provider,
    "* { background-image: url('../assets/images/bg_login.png'); background-size: cover; background-position: center; }"
    "#register-box { background-color: #FFFFFF; border-radius: 20px; padding: 48px 72px; }"
    "#register-title { font-family: Poppins; font-size: 28px; font-weight: 600; color: #101828; background-color: transparent; }"
    "#email-label, #password-label, #username-label, #phone-label, #confirm-password-label { font-family: Poppins; font-size: 18px; font-weight: 500; color: #344054; background-color: transparent; }"
    "#email-entry, #password-entry, #username-entry, #phone-entry, #confirm-password-entry  { border: 3px solid #D0D5DD; border-radius: 8px; padding: 12px 16px; color: #344054; background-color: #FFFFFF; }"
    "#register-button { color: #FCFCFD; border-radius: 8px; font-family: Poppins; font-weight: 600; font-size: 16px; background-color: #1570EF;}"
    "#register-button:hover { background-color: #125ECB; }"
    "#footer-text, #sign-up { font-family: Poppins; font-size: 16px; font-weight: 400; color: #98A2B3; background-color: transparent; }"
    "#sign-up { color: #1570EF; text-decoration: underline; }",
    -1, NULL);
    GtkWidget *register_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
    gtk_widget_set_name(register_box, "register-box");

    GtkWidget *label_register_title = gtk_label_new("Create an account");
    gtk_widget_set_name(label_register_title, "register-title");
    gtk_box_pack_start(GTK_BOX(register_box), label_register_title, FALSE, FALSE, 0);

    GtkWidget *label_username = gtk_label_new("Username");
    gtk_widget_set_name(label_username, "username-label");
    gtk_box_pack_start(GTK_BOX(register_box), label_username, FALSE, FALSE, 0);

    entry_username = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), "Enter your username");
    gtk_widget_set_name(entry_username, "username-entry");
    gtk_box_pack_start(GTK_BOX(register_box), entry_username, FALSE, FALSE, 0);

    GtkWidget *label_phone = gtk_label_new("Phone Number");
    gtk_widget_set_name(label_phone, "phone-label");
    gtk_box_pack_start(GTK_BOX(register_box), label_phone, FALSE, FALSE, 0);

    entry_phone = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_phone), "Enter your phone number");
    gtk_widget_set_name(entry_phone, "phone-entry");
    gtk_box_pack_start(GTK_BOX(register_box), entry_phone, FALSE, FALSE, 0);

    GtkWidget *label_email = gtk_label_new("Email");
    gtk_widget_set_name(label_email, "email-label");
    gtk_box_pack_start(GTK_BOX(register_box), label_email, FALSE, FALSE, 0);

    entry_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Enter your email");
    gtk_widget_set_name(entry_email, "email-entry");
    gtk_box_pack_start(GTK_BOX(register_box), entry_email, FALSE, FALSE, 0);

    GtkWidget *label_password = gtk_label_new("Password");
    gtk_widget_set_name(label_password, "password-label");
    gtk_box_pack_start(GTK_BOX(register_box), label_password, FALSE, FALSE, 0);

    entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Enter your password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_widget_set_name(entry_password, "password-entry");
    gtk_box_pack_start(GTK_BOX(register_box), entry_password, FALSE, FALSE, 0);

    GtkWidget *label_confirm_password = gtk_label_new("Confirm Password");
    gtk_widget_set_name(label_confirm_password, "confirm-password-label");
    gtk_box_pack_start(GTK_BOX(register_box), label_confirm_password, FALSE, FALSE, 0);

    entry_confirm_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_confirm_password), "Re-enter your password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_confirm_password), FALSE);
    gtk_widget_set_name(entry_confirm_password, "confirm-password-entry");
    gtk_box_pack_start(GTK_BOX(register_box), entry_confirm_password, FALSE, FALSE, 0);

    GtkWidget *button_register = gtk_button_new_with_label("Create account");
    gtk_widget_set_name(button_register, "register-button");
    g_signal_connect(button_register, "clicked", G_CALLBACK(on_register), register_box);
    gtk_box_pack_start(GTK_BOX(register_box), button_register, FALSE, FALSE, 0);

    label_status = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(register_box), label_status, FALSE, FALSE, 0);

    GtkWidget *hbox_footer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *footer_text = gtk_label_new("Already have an account?");
    gtk_widget_set_name(footer_text, "footer-text");
    gtk_box_pack_start(GTK_BOX(hbox_footer), footer_text, FALSE, FALSE, 0);

    GtkWidget *login_button = gtk_button_new_with_label("Log in");
    gtk_widget_set_name(login_button, "login-link");
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_link_click), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_footer), login_button, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(register_box), hbox_footer, FALSE, FALSE, 0);

    return register_box;
}
