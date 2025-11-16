#include "login.h"
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../auth/auth.h"
#include "../homepage/homepage.h"
#include "../register/register.h"
#include "../global/global.h"

void on_login_clicked(GtkWidget *widget, gpointer data) {
    const char *email = gtk_entry_get_text(GTK_ENTRY(entry_email));
    const char *password = gtk_entry_get_text(GTK_ENTRY(entry_password));

    if (strlen(email) == 0 || strlen(password) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Please fill in all fields!");
        return;
    }

    if (!validate_email(email)) {
        gtk_label_set_text(GTK_LABEL(label_status), "Email is not valid!");
        return;
    }
    strcpy(email_user, email);
    snprintf(buffer, MAX_LENGTH, "LOGIN %s:%s", email, password);
    send(sock, buffer, sizeof(buffer), 0);
    g_print("Sent to server: %s\n", buffer);
    recv(sock, buffer, sizeof(buffer), 0);
    g_print("Received from server: %s\n", buffer);
    if (strncmp(buffer, "SUCCESS:", 8) == 0){
        sscanf(buffer + 8, "%d", &user_id);
        printf("User ID: %d\n", user_id);
        // if (get_list_tickets_ordered() == -1){
        //     printf("Get tickets ordered fail\n");
        // }
        GtkWidget *homepage_widget = create_homepage_window();
        set_content(homepage_widget);
    }
    else if (strcmp(buffer, "EMAIL_NOT_FOUND") == 0){
        gtk_label_set_text(GTK_LABEL(label_status), "Email not found, please register email");
        return;
    }
    else {
        gtk_label_set_text(GTK_LABEL(label_status), "Login failed, please try again");
        return;
    }

}

void on_register_link_click(GtkWidget *widget, gpointer data) {
    GtkWidget *register_content = create_register_window();
    set_content(register_content);
}

GtkWidget *create_login_window() {
    GtkWidget *overlay, *login_box, *button_login, *hbox_footer;

   GtkCssProvider *provider = gtk_css_provider_new();
gtk_css_provider_load_from_data(provider,
    "* { background-image: url('../assets/images/bg_login.png'); background-size: cover; background-position: center; }"
    "#login-box { background-color: #FFFFFF; border-radius: 20px; padding: 48px 72px; }"
    "#login-title { font-family: Poppins; font-size: 28px; font-weight: 600; color: #101828; background-color: transparent; }"
    "#email-label, #password-label { font-family: Poppins; font-size: 18px; font-weight: 500; color: #344054; background-color: transparent; }"
    "#email-entry, #password-entry { border: 3px solid #D0D5DD; border-radius: 8px; padding: 12px 16px; color: #344054; background-color: #FFFFFF; }"
    "#login-button { color: #FCFCFD; border-radius: 8px; font-family: Poppins; font-weight: 600; font-size: 16px; background-color: #1570EF;}"
    "#login-button:hover { background-color: #125ECB; }"
    "#footer-text, #sign-up { font-family: Poppins; font-size: 16px; font-weight: 400; color: #98A2B3; background-color: transparent; }"
    "#sign-up { color: #1570EF; text-decoration: underline; }",
    -1, NULL);


    login_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
    gtk_widget_set_name(login_box, "login-box");
    GtkStyleContext *login_box_context = gtk_widget_get_style_context(login_box);
    gtk_style_context_add_provider(login_box_context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GtkWidget *label_login_title = gtk_label_new("Login to your account");
    gtk_widget_set_name(label_login_title, "login-title");
    gtk_box_pack_start(GTK_BOX(login_box), label_login_title, FALSE, FALSE, 0);

    GtkWidget *label_email = gtk_label_new("Email");
    gtk_widget_set_name(label_email, "email-label");
    gtk_box_pack_start(GTK_BOX(login_box), label_email, FALSE, FALSE, 0);

    entry_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Enter your email");
    gtk_widget_set_name(entry_email, "email-entry");
    gtk_box_pack_start(GTK_BOX(login_box), entry_email, FALSE, FALSE, 0);

    GtkWidget *label_password = gtk_label_new("Password");
    gtk_widget_set_name(label_password, "password-label");
    gtk_box_pack_start(GTK_BOX(login_box), label_password, FALSE, FALSE, 0);

    entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Enter your password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_widget_set_name(entry_password, "password-entry");
    gtk_box_pack_start(GTK_BOX(login_box), entry_password, FALSE, FALSE, 0);

    label_status = gtk_label_new("");
    gtk_widget_set_name(label_status, "status-label");
    gtk_box_pack_start(GTK_BOX(login_box), label_status, FALSE, FALSE, 0);

    button_login = gtk_button_new_with_label("Login now");
    gtk_widget_set_name(button_login, "login-button");
    g_signal_connect(button_login, "clicked", G_CALLBACK(on_login_clicked), login_box);
    gtk_box_pack_start(GTK_BOX(login_box), button_login, FALSE, FALSE, 0);

    
    hbox_footer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *footer_text = gtk_label_new("Don't have an account?");
    gtk_widget_set_name(footer_text, "footer-text");
    gtk_box_pack_start(GTK_BOX(hbox_footer), footer_text, FALSE, FALSE, 0);

    GtkWidget *sign_up = gtk_button_new_with_label("Sign up?");
    gtk_widget_set_name(sign_up, "sign-up");
    gtk_box_pack_start(GTK_BOX(hbox_footer), sign_up, FALSE, FALSE, 0);
    g_signal_connect(sign_up, "clicked", G_CALLBACK(on_register_link_click),login_box);

    gtk_box_pack_start(GTK_BOX(login_box), hbox_footer, FALSE, FALSE, 0);

    return login_box;
}
